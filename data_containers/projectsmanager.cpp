/*
    Zarządca projektów
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

#include <QTimer>
#include <QStandardItemModel>
#include <QDebug>

#include "projectinfo.hpp"
#include "projectsmanager.hpp"
#include "widgets/projectinfowidget.hpp"
#include "releaseinfo.hpp"

ProjectsManager::ProjectsManager(): id(0)
{
  model=new QStandardItemModel();
}

ProjectsManager::~ProjectsManager()
{
  delete model;
}


ProjectsManager* ProjectsManager::instance()
{
  static ProjectsManager _instance;
  return &_instance;
}


///TODO: przywrócić nazwy itemów oraz uczynić wydania childrenami (patrz widgetlist.cpp)
void ProjectsManager::registerProject(ProjectInfo* project)
{
  projectsList.append(project);
  
  //zaktualizuj model
  QStandardItem *projectItem=new QStandardItem(project->getName());
  
  //zapisz id projektu
  projectItem->setData(project->getId());   
  
  qDebug() << QString("registering project %1 with id %2").arg(project->getName()).arg(project->getId());
  
  foreach(const ReleaseInfo *release, *project->getReleasesList())
  {
    QStandardItem *releaseItem=new QStandardItem(release->getName());
    releaseItem->setData(release->getId());  //zapisz id wydania
    projectItem->appendRow(releaseItem);     //dopisz wydanie do projektu
  }
  model->appendRow(projectItem);             //dopisz projekt do modelu
}


void ProjectsManager::setProjectInfoWidget(ProjectInfoWidget* piW)
{
  projectInfoWidget=piW;
}


void ProjectsManager::showInfo(ReleaseInfo* releaseInfo)
{
  if (projectInfoWidget)
    projectInfoWidget->setProjectRelease(releaseInfo);
}


int ProjectsManager::getId()
{
  return id++;
}


QStandardItemModel* ProjectsManager::getModel()
{
  return model;
}


ProjectInfo* ProjectsManager::findProject(int projectId)
{
  ProjectInfo *ret=0;
  
  foreach(ProjectInfo *projectInfo, projectsList)
  {
    if (projectInfo->getId()==projectId)
    {
      ret=projectInfo;
      break;
    }
  }
  
  return ret;
}


void ProjectsManager::destroyProjects()
{
  while (projectsList.count()>0)
    delete projectsList.takeFirst();
}
