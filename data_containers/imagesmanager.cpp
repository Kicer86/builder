/*
    <one line to give the program's name and a brief idea of what it does.>
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

#include <QImage>

#include "misc/functions.hpp"
#include "imagesmanager.hpp"

ImagesManager::ImagesManager()
{

}


ImagesManager* ImagesManager::instance()
{
  static ImagesManager _instance;
  return &_instance;
}


ImageWidget::ImageLayerPtr ImagesManager::getImage(const QString& name, int res)
{
  bool svg=name.right(4).toLower()==".svg";

  if (svg)
    return ImageWidget::ImageLayerPtr(new ImageLayer(Functions::dataPath(QString("icons/svg/%1").arg(name)), res, res));
  else
    return ImageWidget::ImageLayerPtr(new ImageLayer(Functions::dataPath( QString("icons/%1x%1/%2").arg(res).arg(name) )));
}
