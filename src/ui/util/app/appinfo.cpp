#include "appinfo.h"

#include "appiconprovider.h"
#include "apputil.h"

QString AppInfo::iconPath() const
{
    return AppIconProvider::iconPath(iconId);
}

bool AppInfo::isFileModified(const QString &appPath) const
{
    return !fileModTime.isNull()
            && fileModTime != AppUtil::getModTime(appPath);
}
