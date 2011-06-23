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

#include "stdafx.h"

#include "builder-config.h"
#include "functions.hpp"


QString sizeToString(int value)
{
  QString bps="B/s";

  if (value>1024*1024)
  {
    value/=1024*1024;
    bps="MiB";
  }
  else if (value>1024)
  {
    value/=1024;
    bps="KiB";
  }
  
  return QString("%1%2").arg(value).arg(bps);
}


QString setColour(const QString &str, const QColor &colour)
{
  return QString("<span style=\"color: %1;\">%2</span>").arg(colour.name()).arg(str);
}


QString dataPath(const QString &path)
{
  return QString("%1/%2").arg(BUILDER_DATADIR, path);
}
