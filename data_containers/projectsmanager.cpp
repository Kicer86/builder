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

#include <QDebug>
#include <QStandardItem>

#include <debug.hpp>

#include "projectinfo.hpp"
#include "projectsmanager.hpp"
#include "widgets/projectinfowidget.hpp"
#include "releaseinfo.hpp"

ProjectsManager::ProjectsManager(): id(0)
{
    model = new QStandardItemModel();
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

    qDebug() << QString("registering project %1 with id %2").arg(project->getName()).arg(project->getId());

    foreach(const ReleaseInfo *release, *project->getReleasesList())
    {
        QStandardItem *releaseItem = new QStandardItem(release->getName());
        releaseItem->setData(release->getId());  //zapisz id wydania
        model->appendRow(releaseItem);
    }
}


void ProjectsManager::setProjectInfoWidget(ProjectInfoWidget* piW)
{
    projectInfoWidget = piW;
}


void ProjectsManager::showInfo(ReleaseInfo* releaseInfo)
{
    if (projectInfoWidget)
        projectInfoWidget->setRelease(releaseInfo);
}


int ProjectsManager::getId()
{
    return id++;
}


QStandardItemModel* ProjectsManager::getModel() const
{
    return model;
}


ReleaseInfo* ProjectsManager::getCurrentRelease() const
{
    if (projectInfoWidget)
        return projectInfoWidget->getCurrentRelease();
    else
        return 0;
}



ProjectInfo* ProjectsManager::findProject(int projectId)
{
    ProjectInfo *ret = 0;

    foreach(ProjectInfo *projectInfo, projectsList)
    {
        if (projectInfo->getId() == projectId)
        {
            ret = projectInfo;
            break;
        }
    }

    return ret;
}


ReleaseInfo* ProjectsManager::findRelease(int releaseId)
{
    ReleaseInfo *ret = 0;

    foreach( ProjectInfo *projectInfo, projectsList )
        foreach( ReleaseInfo *releaseInfo, *(projectInfo->getReleasesList()) )
            if (releaseInfo->getId() == releaseId)
            {
                ret = releaseInfo;
                goto quit;
            }

quit:
    return ret;
}


void ProjectsManager::destroyProjects()
{
    while (projectsList.count() > 0)
        delete projectsList.takeFirst();
}


void ProjectsManager::copyRelease(const ReleaseInfo &releaseInfo)
{
    debug(DebugLevel::Info) << "copying release " << releaseInfo.getName()
                            << " of project " << releaseInfo.getProjectInfo()->getName();
}
