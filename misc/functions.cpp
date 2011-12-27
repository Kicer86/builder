/*
    Zbiór funkcji różnego przeznaczenia
    Copyright (C) 2011  Michał Walenciak <Kicer86@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QString>
#include <QColor>
#include <QModelIndex>

#include "builder-config.h"
#include "functions.hpp"
#include "data_containers/releaseinfo.hpp"
#include "data_containers/projectinfo.hpp"

namespace Builder
{

QString sizeToString(int value)
{
  QString suffix="B";

  if (value>1024*1024)
  {
    value/=1024*1024;
    suffix="MiB";
  }
  else if (value>1024)
  {
    value/=1024;
    suffix="KiB";
  }

  return QString("%1%2").arg(value).arg(suffix);
}


QString setColour(const QString &str, const QColor &colour)
{
  return QString("<span style=\"color: %1;\">%2</span>").arg(colour.name()).arg(str);
}


QString dataPath(const QString &path)
{
  return QString("%1/%2").arg(BUILDER_DATADIR, path);
}


ReleaseInfo* getReleaseInfo(const QModelIndex &index)
{
    ReleaseInfo *result;

    result = reinterpret_cast<ReleaseInfo *>(index.data(Qt::UserRole + 1).value<void *>());

    return result;
}


const ProjectInfo* getProjectInfo(const QModelIndex &index)
{
    const ProjectInfo *result = getReleaseInfo(index)->getProjectInfo();

    return result;
}

} //namespace Builder
