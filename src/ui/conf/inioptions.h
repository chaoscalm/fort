#ifndef INIOPTIONS_H
#define INIOPTIONS_H

#include <QRect>

#include "../util/json/mapwrapper.h"

class FortSettings;

#define DEFAULT_APP_GROUP_BITS         0xFFFF
#define DEFAULT_MONTH_START            1
#define DEFAULT_TRAF_HOUR_KEEP_DAYS    90 // ~3 months
#define DEFAULT_TRAF_DAY_KEEP_DAYS     365 // ~1 year
#define DEFAULT_TRAF_MONTH_KEEP_MONTHS 36 // ~3 years
#define DEFAULT_LOG_IP_KEEP_COUNT      10000

class IniOptions : public MapWrapper
{
public:
    explicit IniOptions() = default;
    explicit IniOptions(const IniOptions &o);

    FortSettings *settings() const { return m_settings; }
    void setSettings(FortSettings *v) { m_settings = v; }

    bool logDebug() const { return valueBool("base/debug"); }
    void setLogDebug(bool v) { setValue("base/debug", v); }

    bool logConsole() const { return valueBool("base/console"); }
    void setLogConsole(bool v) { setValue("base/console", v); }

    bool hotKeyEnabled() const { return valueBool("hotKey/enabled"); }
    void setHotKeyEnabled(bool v) { setValue("hotKey/enabled", v); }

    bool hasPassword() const { return valueBool("base/hasPassword_"); }
    void setHasPassword(bool v) { setValue("base/hasPassword_", v); }

    QString password() const { return valueText("base/password_"); }
    void setPassword(const QString &v) { setValue("base/password_", v); }

    bool startupModeSet() const { return contains("ext/startupMode_"); }
    void cacheStartupMode(qint8 v) { setCache("ext/startupMode_", v); }

    qint8 startupMode() const { return valueInt("ext/startupMode_"); }
    void setStartupMode(qint8 v) { setValue("ext/startupMode_", v); }

    bool explorerIntegratedSet() const { return contains("ext/explorerIntegrated_"); }
    void cacheExplorerIntegrated(bool v) { setCache("ext/explorerIntegrated_", v); }

    bool explorerIntegrated() const { return valueBool("ext/explorerIntegrated_"); }
    void setExplorerIntegrated(bool v) { setValue("ext/explorerIntegrated_", v); }

    bool taskInfoListSet() const { return contains("task/infoList_"); }

    QVariant taskInfoList() const { return value("task/infoList_"); }
    void setTaskInfoList(const QVariant &v) { setValue("task/infoList_", v); }

    QString defaultLanguage() const;

    QString language() const { return valueText("base/language", defaultLanguage()); }
    void setLanguage(const QString &v) { setValue("base/language", v); }

    QString hotKeyPrograms() const { return valueText("hotKey/programs"); }
    QString hotKeyOptions() const { return valueText("hotKey/options"); }
    QString hotKeyZones() const { return valueText("hotKey/zones"); }
    QString hotKeyGraph() const { return valueText("hotKey/graph"); }
    QString hotKeyConnections() const { return valueText("hotKey/connections"); }
    QString hotKeyFilter() const { return valueText("hotKey/filter", "Ctrl+Alt+Shift+F"); }
    QString hotKeyStopTraffic() const { return valueText("hotKey/stopTraffic"); }
    QString hotKeyStopInetTraffic() const { return valueText("hotKey/stopInetTraffic"); }
    QString hotKeyAllowAllNew() const { return valueText("hotKey/allowAllNew"); }
    QString hotKeyAppGroupModifiers() const
    {
        return valueText("hotKey/appGroupModifiers", "Ctrl+Alt+Shift");
    }
    QString hotKeyQuit() const { return valueText("hotKey/quit"); }

    qint32 quotaDayAlerted() const { return valueInt("quota/dayAlerted"); }
    void setQuotaDayAlerted(qint32 v) { setValue("quota/dayAlerted", v); }

    qint32 quotaMonthAlerted() const { return valueInt("quota/monthAlerted"); }
    void setQuotaMonthAlerted(qint32 v) { setValue("quota/monthAlerted", v); }

    quint32 quotaDayMb() const { return valueUInt("quota/quotaDayMb"); }
    void setQuotaDayMb(quint32 v) { setValue("quota/quotaDayMb", v); }

    quint32 quotaMonthMb() const { return valueUInt("quota/quotaMonthMb"); }
    void setQuotaMonthMb(quint32 v) { setValue("quota/quotaMonthMMb", v); }

    int monthStart() const { return valueInt("stat/monthStart", DEFAULT_MONTH_START); }
    void setMonthStart(int v) { setValue("stat/monthStart", v); }

    int trafHourKeepDays() const
    {
        return valueInt("stat/trafHourKeepDays", DEFAULT_TRAF_HOUR_KEEP_DAYS);
    }
    void setTrafHourKeepDays(int v) { setValue("stat/trafHourKeepDays", v); }

    int trafDayKeepDays() const
    {
        return valueInt("stat/trafDayKeepDays", DEFAULT_TRAF_DAY_KEEP_DAYS);
    }
    void setTrafDayKeepDays(int v) { setValue("stat/trafDayKeepDays", v); }

    int trafMonthKeepMonths() const
    {
        return valueInt("stat/trafMonthKeepMonths", DEFAULT_TRAF_MONTH_KEEP_MONTHS);
    }
    void setTrafMonthKeepMonths(int v) { setValue("stat/trafMonthKeepMonths", v); }

    int trafUnit() const { return valueInt("stat/trafUnit"); }
    void setTrafUnit(int v) { setValue("stat/trafUnit", v); }

    int allowedIpKeepCount() const
    {
        return valueInt("stat/allowedIpKeepCount", DEFAULT_LOG_IP_KEEP_COUNT);
    }
    void setAllowedIpKeepCount(int v) { setValue("stat/allowedIpKeepCount", v); }

    int blockedIpKeepCount() const
    {
        return valueInt("stat/blockedIpKeepCount", DEFAULT_LOG_IP_KEEP_COUNT);
    }
    void setBlockedIpKeepCount(int v) { setValue("stat/blockedIpKeepCount", v); }

    QRect progWindowGeometry() const { return value("progWindow/geometry").toRect(); }
    void setProgWindowGeometry(const QRect &v) { setValue("progWindow/geometry", v); }

    bool progWindowMaximized() const { return valueBool("progWindow/maximized"); }
    void setProgWindowMaximized(bool on) { setValue("progWindow/maximized", on); }

    bool progAppsSortDesc() const { return valueBool("progWindow/appsSortDesc"); }
    void setProgSortDesc(bool v) { setValue("progWindow/appsSortDesc", v); }

    int progAppsSortColumn() const { return valueInt("progWindow/appsSortColumn"); }
    void setProgSortColumn(int v) { setValue("progWindow/appsSortColumn", v); }

    int progAppsHeaderVersion() const { return valueInt("progWindow/appsHeaderVersion"); }
    void setProgAppsHeaderVersion(int v) { setValue("progWindow/appsHeaderVersion", v); }

    QByteArray progAppsHeader() const { return valueByteArray("progWindow/appsHeader"); }
    void setProgAppsHeader(const QByteArray &v) { setValue("progWindow/appsHeader", v); }

    QRect optWindowGeometry() const { return value("optWindow/geometry").toRect(); }
    void setOptWindowGeometry(const QRect &v) { setValue("optWindow/geometry", v); }

    bool optWindowMaximized() const { return valueBool("optWindow/maximized"); }
    void setOptWindowMaximized(bool on) { setValue("optWindow/maximized", on); }

    QByteArray optWindowAddrSplit() const { return valueByteArray("optWindow/addrSplit"); }
    void setOptWindowAddrSplit(const QByteArray &v) { setValue("optWindow/addrSplit", v); }

    QByteArray optWindowAppsSplit() const { return valueByteArray("optWindow/appsSplit"); }
    void setOptWindowAppsSplit(const QByteArray &v) { setValue("optWindow/appsSplit", v); }

    QByteArray optWindowStatSplit() const { return valueByteArray("optWindow/statSplit"); }
    void setOptWindowStatSplit(const QByteArray &v) { setValue("optWindow/statSplit", v); }

    QRect zoneWindowGeometry() const { return value("zoneWindow/geometry").toRect(); }
    void setZoneWindowGeometry(const QRect &v) { setValue("zoneWindow/geometry", v); }

    bool zoneWindowMaximized() const { return valueBool("zoneWindow/maximized"); }
    void setZoneWindowMaximized(bool on) { setValue("zoneWindow/maximized", on); }

    int zonesHeaderVersion() const { return valueInt("zoneWindow/zonesHeaderVersion"); }
    void setZonesHeaderVersion(int v) { setValue("zoneWindow/zonesHeaderVersion", v); }

    QByteArray zonesHeader() const { return valueByteArray("zoneWindow/zonesHeader"); }
    void setZonesHeader(const QByteArray &v) { setValue("zoneWindow/zonesHeader", v); }

    bool graphWindowVisible() const { return valueBool("graphWindow/visible"); }
    void setGraphWindowVisible(bool on) { setValue("graphWindow/visible", on); }

    QRect graphWindowGeometry() const { return value("graphWindow/geometry").toRect(); }
    void setGraphWindowGeometry(const QRect &v) { setValue("graphWindow/geometry", v); }

    bool graphWindowMaximized() const { return valueBool("graphWindow/maximized"); }
    void setGraphWindowMaximized(bool on) { setValue("graphWindow/maximized", on); }

    bool graphWindowAlwaysOnTop() const { return valueBool("graphWindow/alwaysOnTop", true); }
    void setGraphWindowAlwaysOnTop(bool on) { setValue("graphWindow/alwaysOnTop", on); }

    bool graphWindowFrameless() const { return valueBool("graphWindow/frameless"); }
    void setGraphWindowFrameless(bool on) { setValue("graphWindow/frameless", on); }

    bool graphWindowClickThrough() const { return valueBool("graphWindow/clickThrough"); }
    void setGraphWindowClickThrough(bool on) { setValue("graphWindow/clickThrough", on); }

    bool graphWindowHideOnHover() const { return valueBool("graphWindow/hideOnHover"); }
    void setGraphWindowHideOnHover(bool on) { setValue("graphWindow/hideOnHover", on); }

    int graphWindowOpacity() const { return valueInt("graphWindow/opacity", 90); }
    void setGraphWindowOpacity(int v) { setValue("graphWindow/opacity", v); }

    int graphWindowHoverOpacity() const { return valueInt("graphWindow/hoverOpacity", 95); }
    void setGraphWindowHoverOpacity(int v) { setValue("graphWindow/hoverOpacity", v); }

    int graphWindowMaxSeconds() const { return valueInt("graphWindow/maxSeconds", 500); }
    void setGraphWindowMaxSeconds(int v) { setValue("graphWindow/maxSeconds", v); }

    QColor graphWindowColor() const
    {
        return valueColor("graphWindow/color", QColor(255, 255, 255));
    }
    void setGraphWindowColor(const QColor &v) { setColor("graphWindow/color", v); }

    QColor graphWindowColorIn() const
    {
        return valueColor("graphWindow/colorIn", QColor(52, 196, 84));
    }
    void setGraphWindowColorIn(const QColor &v) { setColor("graphWindow/colorIn", v); }

    QColor graphWindowColorOut() const
    {
        return valueColor("graphWindow/colorOut", QColor(235, 71, 63));
    }
    void setGraphWindowColorOut(const QColor &v) { setColor("graphWindow/colorOut", v); }

    QColor graphWindowAxisColor() const
    {
        return valueColor("graphWindow/axisColor", QColor(0, 0, 0));
    }
    void setGraphWindowAxisColor(const QColor &v) { setColor("graphWindow/axisColor", v); }

    QColor graphWindowTickLabelColor() const
    {
        return valueColor("graphWindow/tickLabelColor", QColor(0, 0, 0));
    }
    void setGraphWindowTickLabelColor(const QColor &v)
    {
        setColor("graphWindow/tickLabelColor", v);
    }

    QColor graphWindowLabelColor() const
    {
        return valueColor("graphWindow/labelColor", QColor(0, 0, 0));
    }
    void setGraphWindowLabelColor(const QColor &v) { setColor("graphWindow/labelColor", v); }

    QColor graphWindowGridColor() const
    {
        return valueColor("graphWindow/gridColor", QColor(200, 200, 200));
    }
    void setGraphWindowGridColor(const QColor &v) { setColor("graphWindow/gridColor", v); }

    QRect connWindowGeometry() const { return value("connWindow/geometry").toRect(); }
    void setConnWindowGeometry(const QRect &v) { setValue("connWindow/geometry", v); }

    bool connWindowMaximized() const { return valueBool("connWindow/maximized"); }
    void setConnWindowMaximized(bool on) { setValue("connWindow/maximized", on); }

    int connListHeaderVersion() const { return valueInt("connWindow/connListHeaderVersion"); }
    void setConnListHeaderVersion(int v) { setValue("connWindow/connListHeaderVersion", v); }

    QByteArray connListHeader() const { return valueByteArray("connWindow/connListHeader"); }
    void setConnListHeader(const QByteArray &v) { setValue("connWindow/connListHeader", v); }

    bool connAutoScroll() const { return valueBool("connWindow/autoScroll"); }
    void setConnAutoScroll(bool on) { setValue("connWindow/autoScroll", on); }

    bool connShowHostNames() const { return valueBool("connWindow/showHostNames"); }
    void setConnShowHostNames(bool on) { setValue("connWindow/showHostNames", on); }

public slots:
    void save(FortSettings *settings) const;

protected:
    QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const override;
    void setCache(const QString &key, const QVariant &v) const;

    static bool isTransientKey(const QString &key);

private:
    FortSettings *m_settings = nullptr;
};

#endif // INIOPTIONS_H
