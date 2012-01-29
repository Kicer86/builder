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

#include <assert.h>

#include "widgetlistproxymodel.hpp"
#include "data_containers/projectsmanager.hpp"
#include "data_containers/projectinfo.hpp"
#include "data_containers/releaseinfo.hpp"
#include "misc/functions.hpp"

WidgetListProxyModel::WidgetListProxyModel(QObject* p): QSortFilterProxyModel(p)
{}


WidgetListProxyModel::~WidgetListProxyModel()
{}


//http://doc.qt.nokia.com/latest/qsortfilterproxymodel.html#details
bool WidgetListProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
  //owning project
  const ProjectInfo *lpi = Functions::getProjectInfo(left);
  const ProjectInfo *rpi = Functions::getProjectInfo(right);

  //same project?
  if (lpi == rpi)
  {
      ReleaseInfo *lri = Functions::getReleaseInfo(left);
      ReleaseInfo *rri = Functions::getReleaseInfo(right);

      assert ( lri != nullptr || rri != nullptr );  //there cannot be two empty releases in the same project

      if (lri == nullptr)
          return true;

      if (rri == nullptr)
          return false;

      //build state
      bool buildLeft = lri->getBuildFlag();
      bool buildRight = rri->getBuildFlag();

      if (buildLeft == buildRight)  //same status of build?
      {
          //check download state
          bool downloadLeft = lri->getDownloadFlag();
          bool downloadRight = rri->getDownloadFlag();

          if (downloadLeft == downloadRight)         //same status of download??
              return lri->getName() < rri->getName();  //use alphabetical order
          else
              return downloadLeft == true && downloadRight == false;
      }
      else
          return buildLeft == true && buildRight == false;
  }
  else //different projects
  {
      //use project's "global" status
      return lpi->getStatus() > rpi->getStatus();
  }
}
