#include "fortmanager.h"

#include <QApplication>
#include <QLoggingCategory>
#include <QMessageBox>
#include <QThreadPool>

#include <fort_version.h>

#include <appinfo/appinfocache.h>
#include <conf/firewallconf.h>
#include <control/controlmanager.h>
#include <driver/drivercommon.h>
#include <form/dialog/passworddialog.h>
#include <fortsettings.h>
#include <hostinfo/hostinfocache.h>
#include <manager/drivelistmanager.h>
#include <manager/envmanager.h>
#include <manager/hotkeymanager.h>
#include <manager/logger.h>
#include <manager/nativeeventfilter.h>
#include <manager/servicemanager.h>
#include <manager/translationmanager.h>
#include <model/zonelistmodel.h>
#include <rpc/appinfomanagerrpc.h>
#include <rpc/askpendingmanagerrpc.h>
#include <rpc/autoupdatemanagerrpc.h>
#include <rpc/confappmanagerrpc.h>
#include <rpc/confmanagerrpc.h>
#include <rpc/confrulemanagerrpc.h>
#include <rpc/confzonemanagerrpc.h>
#include <rpc/dberrormanagerrpc.h>
#include <rpc/drivermanagerrpc.h>
#include <rpc/logmanagerrpc.h>
#include <rpc/quotamanagerrpc.h>
#include <rpc/rpcmanager.h>
#include <rpc/serviceinfomanagerrpc.h>
#include <rpc/statblockmanagerrpc.h>
#include <rpc/statmanagerrpc.h>
#include <rpc/taskmanagerrpc.h>
#include <rpc/windowmanagerfake.h>
#include <task/taskinfozonedownloader.h>
#include <user/usersettings.h>
#include <util/dateutil.h>
#include <util/fileutil.h>
#include <util/ioc/ioccontainer.h>
#include <util/osutil.h>
#include <util/startuputil.h>

namespace {

const QLoggingCategory LC("fortManager");

bool canInstallDriver(FortSettings *settings)
{
    const bool canInstallDriver = (settings->canInstallDriver() || settings->isPortable());
    const bool isAdmin = settings->isUserAdmin();

    return (canInstallDriver && isAdmin);
}

void showErrorMessage(const QString &errorMessage)
{
    IoC<WindowManager>()->showErrorBox(errorMessage);
}

inline void setupMasterServices(IocContainer *ioc, const FortSettings *settings)
{
    ioc->setService(new ConfManager(settings->confFilePath()));
    ioc->setService(new ConfAppManager());
    ioc->setService(new ConfRuleManager());
    ioc->setService(new ConfZoneManager());
    ioc->setService(new QuotaManager());
    ioc->setService(new StatManager(settings->statFilePath()));
    ioc->setService(new StatBlockManager(settings->statBlockFilePath()));
    ioc->setService(new AskPendingManager());
    ioc->setService(new AutoUpdateManager(settings->cachePath()));
    ioc->setService(new DriverManager());
    ioc->setService(new AppInfoManager(settings->cacheFilePath()));
    ioc->setService(new LogManager());
    ioc->setService(new ServiceInfoManager());
    ioc->setService(new TaskManager());
    ioc->setService(new DbErrorManager());

    // For Master only
    ioc->setService(new DriveListManager());
}

inline void setupClientServices(IocContainer *ioc, const FortSettings *settings)
{
    ioc->setService<ConfManager>(new ConfManagerRpc(settings->confFilePath()));
    ioc->setService<ConfAppManager>(new ConfAppManagerRpc());
    ioc->setService<ConfRuleManager>(new ConfRuleManagerRpc());
    ioc->setService<ConfZoneManager>(new ConfZoneManagerRpc());
    ioc->setService<QuotaManager>(new QuotaManagerRpc());
    ioc->setService<StatManager>(new StatManagerRpc(settings->statFilePath()));
    ioc->setService<StatBlockManager>(new StatBlockManagerRpc(settings->statBlockFilePath()));
    ioc->setService<AskPendingManager>(new AskPendingManagerRpc());
    ioc->setService<AutoUpdateManager>(new AutoUpdateManagerRpc(settings->cachePath()));
    ioc->setService<DriverManager>(new DriverManagerRpc());
    ioc->setService<AppInfoManager>(
            new AppInfoManagerRpc(settings->cacheFilePath(), settings->noCache()));
    ioc->setService<LogManager>(new LogManagerRpc());
    ioc->setService<ServiceInfoManager>(new ServiceInfoManagerRpc());
    ioc->setService<TaskManager>(new TaskManagerRpc());
    ioc->setService<DbErrorManager>(new DbErrorManagerRpc());
}

inline void setupServices(IocContainer *ioc, const FortSettings *settings)
{
    if (settings->isMaster()) {
        setupMasterServices(ioc, settings);
    } else {
        setupClientServices(ioc, settings);
    }

    if (settings->hasService()) {
        ioc->setService(new RpcManager());
    }

    if (settings->isService()) {
        ioc->setService<WindowManager>(new WindowManagerFake());

        // For Service only
        ioc->setService(new ServiceManager());
    } else {
        ioc->setService(new WindowManager());

        // For UI only
        ioc->setService(new HotKeyManager());
        ioc->setService(new UserSettings());
        ioc->setService(new TranslationManager());
    }

    ioc->setService(new NativeEventFilter());
    ioc->setService(new AppInfoCache());
    ioc->setService(new HostInfoCache());
    ioc->setService(new ZoneListModel());
}

}

FortManager::FortManager(QObject *parent) : QObject(parent) { }

FortManager::~FortManager()
{
    if (m_initialized) {
        closeDriver();

        emit aboutToDestroy();

        checkRemoveDriver();
    }

    deleteManagers();

    OsUtil::closeMutex(m_instanceMutex);
}

bool FortManager::checkRunningInstance(bool isService, bool isLaunch)
{
    bool isSingleInstance;
    m_instanceMutex =
            OsUtil::createMutex(isService ? "Global\\" APP_BASE : APP_BASE, isSingleInstance);
    if (isSingleInstance)
        return true;

    if (isService) {
        qCWarning(LC) << "Quit due Service is already running!";
    } else if (!isLaunch) {
        if (!IoC<ControlManager>()->postCommand(Control::CommandHome, { "show" })) {
            QMessageBox::warning(nullptr, QString(), tr("Application is already running!"));
        }
    }
    return false;
}

void FortManager::initialize()
{
    m_initialized = true;

    OsUtil::setCurrentThreadName("Main");

    setupThreadPool();
    setupLogger();

    createManagers();

    setupEnvManager();
    setupConfManager();
    setupQuotaManager();
    setupTaskManager();
    setupServiceInfoManager();

    checkReinstallDriver();
    checkStartService();

    setupDriver();
    loadConf();
}

void FortManager::setupThreadPool()
{
    QThreadPool::globalInstance()->setMaxThreadCount(16);
}

void FortManager::setupLogger()
{
    Logger *logger = Logger::instance();

    const auto settings = IoC<FortSettings>();

    logger->setIsService(settings->isService());
    logger->setHasService(settings->hasService());
    logger->setPath(settings->logsPath());
    logger->setForceDebug(settings->forceDebug());
}

void FortManager::updateLogger(const FirewallConf *conf)
{
    if (!conf->iniEdited())
        return;

    Logger *logger = Logger::instance();

    logger->setDebug(conf->ini().logDebug());
    logger->setConsole(conf->ini().logConsole());
}

void FortManager::createManagers()
{
    IocContainer *ioc = IoCPinned();

    const auto settings = IoC<FortSettings>();

    setupServices(ioc, settings);

    if (settings->isMaster()) {
        // TODO: COMPAT: Remove after v4.1.0 (via v4.0.0)
        FileUtil::copyFile(settings->statFilePath(), settings->statBlockFilePath());
    }

    ioc->setUpAll();
}

void FortManager::deleteManagers()
{
    IocContainer *ioc = IoCPinned();

    ioc->tearDownAll();
    ioc->autoDeleteAll();
}

void FortManager::install(const char *arg)
{
    if (!arg)
        return;

    switch (arg[0]) {
    case 'b': { // "boot_filter"
        DriverCommon::provRegister(/*bootFilter=*/true); // Register booted provider
    } break;
    case 'p': { // "portable"
        FortManager::setupPortableResource();
        StartupUtil::setPortable(true);
    } break;
    case 's': { // "service"
        StartupUtil::setAutoRunMode(StartupUtil::StartupAllUsers);
        StartupUtil::setServiceInstalled(true);

        OsUtil::endRestartClients();
    } break;
    case 'e': { // "explorer"
        StartupUtil::setExplorerIntegrated(true);
    } break;
    }
}

void FortManager::uninstall(const char *arg)
{
    StartupUtil::setAutoRunMode(StartupUtil::StartupDisabled); // Remove auto-run
    StartupUtil::setExplorerIntegrated(false); // Remove Windows Explorer integration

    StartupUtil::stopService(ServiceControlStopUninstall); // Quit clients & Stop service
    StartupUtil::setServiceInstalled(false); // Uninstall service

    StartupUtil::clearGlobalExplorerIntegrated(); // COMPAT: Remove Global Windows Explorer
                                                  // integration

    if (!arg) {
        DriverCommon::provUnregister(); // Unregister booted provider
    }
}

bool FortManager::installDriver()
{
    const bool hasService = IoC<FortSettings>()->hasService();

    if (hasService) {
        StartupUtil::stopService();
    } else {
        closeDriver();
    }

    IoC<DriverManager>()->reinstallDriver();

    if (hasService) {
        StartupUtil::setServiceInstalled();
    } else {
        // Re-open the driver device and initialize it
        if (setupDriver()) {
            updateDriverConf();
        }
    }

    return true;
}

bool FortManager::removeDriver()
{
    closeDriver();

    IoC<DriverManager>()->uninstallDriver();

    return true;
}

bool FortManager::setupDriver()
{
    auto driverManager = IoC<DriverManager>();
    auto confManager = IoC<ConfManager>();

    bool ok = driverManager->openDevice();

    if (ok && !confManager->validateDriver()) {
        driverManager->closeDevice();
        ok = false;
    }

    if (ok) {
        confManager->updateServices();
    }

    return ok;
}

void FortManager::closeDriver()
{
    updateLogManager(false);
    updateStatManager(nullptr);

    IoC<DriverManager>()->closeDevice();

    QCoreApplication::sendPostedEvents(this);
}

void FortManager::checkRemoveDriver()
{
    const auto settings = IoC<FortSettings>();

    if (!canInstallDriver(settings))
        return;

    if (settings->isService() || StartupUtil::isServiceInstalled())
        return;

    removeDriver();
}

void FortManager::checkReinstallDriver()
{
    const auto settings = IoC<FortSettings>();

    if (!settings->hasService() && canInstallDriver(settings)) {
        IoC<DriverManager>()->checkReinstallDriver();
    }
}

void FortManager::checkStartService()
{
    const auto settings = IoC<FortSettings>();

    if (settings->isMaster() || StartupUtil::isServiceRunning())
        return;

    const bool canStartService = settings->canStartService();
    const bool isAdmin = settings->isUserAdmin();

    if (canStartService && isAdmin) {
        StartupUtil::startService();
    }
}

void FortManager::setupEnvManager()
{
    auto envManager = IoC<EnvManager>();

    connect(IoC<NativeEventFilter>(), &NativeEventFilter::environmentChanged, envManager,
            &EnvManager::onEnvironmentChanged);

    connect(envManager, &EnvManager::environmentUpdated, this, [&] { updateDriverConf(); });
}

void FortManager::setupConfManager()
{
    connect(IoC<ConfManager>(), &ConfManager::confChanged, this, [&](bool onlyFlags) {
        const FirewallConf *conf = IoC<ConfManager>()->conf();

        updateLogger(conf);

        if (!onlyFlags || conf->flagsEdited()) {
            updateDriverConf(onlyFlags);
        }
    });
}

void FortManager::setupQuotaManager()
{
    connect(IoC<QuotaManager>(), &QuotaManager::alert, this, [&](qint8 alertType) {
        IoC<WindowManager>()->showInfoBox(
                QuotaManager::alertTypeText(alertType), tr("Quota Alert"));
    });
}

void FortManager::setupTaskManager()
{
    auto taskManager = IoC<TaskManager>();

    connect(taskManager, &TaskManager::appVersionDownloaded, this, [&](const QString &version) {
        IoC<WindowManager>()->showTrayMessage(tr("New version v%1 available!").arg(version),
                WindowManager::TrayMessageNewVersion);
    });

    connect(taskManager, &TaskManager::zonesDownloaded, this, [&](const QStringList &zoneNames) {
        IoC<WindowManager>()->showTrayMessage(
                tr("Zone Addresses Updated: %1.").arg(zoneNames.join(", ")),
                WindowManager::TrayMessageZones);
    });

    connect(taskManager, &TaskManager::zonesUpdated, IoC<ConfZoneManager>(),
            &ConfZoneManager::updateDriverZones);

    connect(taskManager, &TaskManager::taskDoubleClicked, this, [&](qint8 taskType) {
        auto windowManager = IoC<WindowManager>();

        switch (taskType) {
        case TaskInfo::UpdateChecker: {
            windowManager->showHomeWindowAbout();
        } break;
        case TaskInfo::ZoneDownloader: {
            windowManager->showZonesWindow();
        } break;
        case TaskInfo::AppPurger: {
            windowManager->showProgramsWindow();
        } break;
        }
    });
}

void FortManager::setupServiceInfoManager()
{
    auto serviceInfoManager = IoC<ServiceInfoManager>();

    connect(serviceInfoManager, &ServiceInfoManager::servicesStarted, IoC<ConfManager>(),
            &ConfManager::updateDriverServices);
}

void FortManager::processRestartRequired(const QString &info)
{
    qCDebug(LC) << "Restart required:" << info;

    if (IoC<FortSettings>()->isService()) {
        IoC<ServiceManager>()->restart();
    } else {
        OsUtil::restart();
    }
}

void FortManager::loadConf()
{
    const auto settings = IoC<FortSettings>();
    const auto confManager = IoC<ConfManager>();

    // Validate migration
    QString viaVersion;
    if (!settings->canMigrate(viaVersion)) {
        showErrorMessage(tr("Please first install Fort Firewall v%1 and save Options from it.")
                                 .arg(viaVersion));
        exit(-1); // Exit the program
    }

    if (!confManager->load()) {
        showErrorMessage(tr("Cannot load Settings"));
    }

    qCDebug(LC) << "Started as"
                << (settings->isService()                   ? "Service"
                                   : settings->hasService() ? "Client"
                                                            : "Program");
}

bool FortManager::updateDriverConf(bool onlyFlags)
{
    auto confManager = IoC<ConfManager>();
    auto confAppManager = IoC<ConfAppManager>();

    updateLogManager(false);

    const bool res = confAppManager->updateDriverConf(onlyFlags);
    if (res) {
        updateStatManager(confManager->conf());
    }

    updateLogManager(true);

    return res;
}

void FortManager::updateLogManager(bool active)
{
    IoC<LogManager>()->setActive(active);
}

void FortManager::updateStatManager(FirewallConf *conf)
{
    IoC<StatManager>()->setConf(conf);
}

void FortManager::setupPortableResource()
{
    Q_INIT_RESOURCE(fort_readme);
}

void FortManager::setupResources()
{
    Q_INIT_RESOURCE(appinfo_migrations);
    Q_INIT_RESOURCE(conf_migrations);
    Q_INIT_RESOURCE(conf_zone);
    Q_INIT_RESOURCE(stat_migrations);

    Q_INIT_RESOURCE(fort_icons);
}
