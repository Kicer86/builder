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


#include "widgetlistproxymodel.hpp"
#include "data_containers/projectsmanager.hpp"
#include "data_containers/projectinfo.hpp"

WidgetListProxyModel::WidgetListProxyModel(QObject* p): QSortFilterProxyModel(p)
{}


WidgetListProxyModel::~WidgetListProxyModel()
{}

//http://doc.qt.nokia.com/latest/qsortfilterproxymodel.html#details
bool WidgetListProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
  int lid=left.data(Qt::UserRole+1).toInt();    //info w: void ProjectsManager::registerProject(ProjectInfo* project)
  int rid=right.data(Qt::UserRole+1).toInt();   //info w: void ProjectsManager::registerProject(ProjectInfo* project)
  
  ProjectInfo *lpi=ProjectsManager::instance()->findProject(lid);
  ProjectInfo *rpi=ProjectsManager::instance()->findProject(rid);
  
  int result=lpi->getStatus()-rpi->getStatus();
  
  if (result==0)
    return QSortFilterProxyModel::lessThan(left, right);
  else
    return result>0;
}
