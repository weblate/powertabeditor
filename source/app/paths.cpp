/*
  * Copyright (C) 2015 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "paths.h"

#include <app/appinfo.h>
#include <QLibraryInfo>
#include <QString>
#include <QStandardPaths>

namespace Paths
{
path getConfigDir()
{
    auto p = fromQString(
        QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));

    // On Linux, ConfigLocation is ~/.config, so append the application name.
#ifdef Q_OS_LINUX
    return p / AppInfo::APPLICATION_ID;
#else
    return p;
#endif
}

path getUserDataDir()
{
    return fromQString(
        QStandardPaths::writableLocation(QStandardPaths::DataLocation));
}

std::vector<path> getDataDirs()
{
    QStringList q_paths =
        QStandardPaths::standardLocations(QStandardPaths::DataLocation);
    q_paths.append(QString::fromStdString(AppInfo::getAbsolutePath("data")));

    std::vector<path> paths;
    for (const QString &p : q_paths)
        paths.push_back(fromQString(p));

    return paths;
}

std::vector<path>
getTranslationDirs()
{
    // Look under a "translations" folder under the data folder.
    std::vector<path> paths = getDataDirs();
    for (path &p : paths)
        p /= "translations";

    // Also check in the Qt translations folder.
    paths.push_back(
        fromQString(QLibraryInfo::location(QLibraryInfo::TranslationsPath)));

    return paths;
}

path getHomeDir()
{
    return fromQString(
        QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
}

path fromQString(const QString &str)
{
    return std::filesystem::u8path(str.toStdString());
}

QString toQString(const path &str)
{
    return QString::fromStdString(str.u8string());
}
}
