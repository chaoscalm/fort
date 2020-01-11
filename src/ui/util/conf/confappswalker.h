#ifndef CONFAPPSWALKER_H
#define CONFAPPSWALKER_H

#include <QObject>

#include <functional>

using walkAppsCallback = bool(int groupIndex, bool useGroupPerm,
    bool blocked, bool alerted, const QString &appPath);

class ConfAppsWalker
{
public:
    virtual bool walkApps(std::function<walkAppsCallback> func) = 0;
};

#endif // CONFAPPSWALKER_H
