#include <QApplication>
#include <QMessageBox>
#include <QStyle>
#include <QStyleFactory>

#ifdef USE_VISUAL_LEAK_DETECTOR
#    include <vld.h>
#endif

#include <fort_version.h>

#include "control/controlmanager.h"
#include "fortcommon.h"
#include "fortmanager.h"
#include "fortsettings.h"
#include "util/envmanager.h"
#include "util/osutil.h"

#define FORT_ERROR_INSTANCE 1
#define FORT_ERROR_CONTROL  2

int main(int argc, char *argv[])
{
    // Uninstall: Unregister booted provider and exit
    if (argc > 1 && !strcmp(argv[1], "-u")) {
        FortCommon::provUnregister();
        return 0;
    }

    FortSettings fortSettings;

    // Process global settings required before QApplication costruction
    fortSettings.setupGlobal();

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);
#endif

    QApplication::setQuitOnLastWindowClosed(false);

    QApplication app(argc, argv);
    QApplication::setApplicationName(APP_NAME);
    QApplication::setApplicationVersion(APP_VERSION_STR);
    QApplication::setApplicationDisplayName(APP_NAME " v" APP_VERSION_STR);

    EnvManager envManager;

    // Initialize with command line arguments
    fortSettings.initialize(QCoreApplication::arguments(), &envManager);

#ifdef USE_CONTROL_COMMANDS
    ControlManager controlManager(QApplication::applicationName(), fortSettings.controlCommand());

    // Send control request to running instance
    if (controlManager.isClient()) {
        return controlManager.post(fortSettings.args()) ? 0 : FORT_ERROR_CONTROL;
    }
#endif

#ifdef APP_SINGLE_INSTANCE
    // Check running instance
    if (!OsUtil::createGlobalMutex(APP_NAME)) {
        QMessageBox::critical(nullptr, QString(), "Application is already running!");
        return FORT_ERROR_INSTANCE;
    }
#endif

    // Style & Palette
    const auto fusionStyle = QStyleFactory::create("Fusion");
    QApplication::setStyle(fusionStyle);
    QApplication::setPalette(fusionStyle->standardPalette());

    FortManager fortManager(&fortSettings, &envManager);
    fortManager.launch();

#ifdef USE_CONTROL_COMMANDS
    // Process control requests from clients
    if (!controlManager.listen(&fortManager)) {
        return FORT_ERROR_CONTROL;
    }
#endif

    return QApplication::exec();
}
