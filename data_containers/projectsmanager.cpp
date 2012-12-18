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
#include <QDir>
#include <QStandardItem>

#include <OpenLib/common/debug.hpp>

#include "projectinfo.hpp"
#include "projectsmanager.hpp"
#include "releaseinfo.hpp"
#include "dialogs/releasecopydialog.hpp"
#include "misc/functions.hpp"
#include "widgets/projectinfowidget.hpp"


ProjectsManager::ProjectsManager()
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


void ProjectsManager::registerProject(ProjectInfo* project)
{
    projectsList.append(project);

    qDebug() << QString("registering project %1").arg(project->getName());

    //create project's item
    QStandardItem *projectItem = new QStandardItem(project->getName());
    Functions::setReleaseInfo(projectItem, project);
    model->appendRow(projectItem);                               //append it

    foreach(ReleaseInfo *release, project->getReleasesList())
    {
        QStandardItem *releaseItem = new QStandardItem(release->getName());
        Functions::setReleaseInfo(releaseItem, release);         //save pointer to ReleaseInfo to QStandardItem
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


QStandardItemModel* ProjectsManager::getModel() const
{
    return model;
}


ReleaseInfo* ProjectsManager::getCurrentRelease() const
{
    if (projectInfoWidget)
        return projectInfoWidget->getCurrentRelease();
    else
        return nullptr;
}


void ProjectsManager::destroyProjects()
{
    while (projectsList.count() > 0)
        delete projectsList.takeFirst();
}


void ProjectsManager::copyRelease(const ReleaseInfo &releaseInfo)
{
    const ProjectInfo *projectInfo = releaseInfo.getProjectInfo();
    debug(DebugLevel::Info) << "copying release " << releaseInfo.getName()
                            << " of project " << projectInfo->getName();

    ReleaseCopyDialog dialog(releaseInfo);
    if (dialog.exec() == QDialog::Accepted)
    {
        //do copy operation
        const QString newName = dialog.getNewName();
        const QString projectPath = projectInfo->getPath();
        const QString sourceReleasePath = releaseInfo.getReleasePath();

        const QDir dir(projectPath);
        dir.mkdir(newName);

        QFile::copy(sourceReleasePath + "/download.lua",
                    projectPath + "/" + newName + "/download.lua");
        QFile::copy(sourceReleasePath + "/" + projectInfo->getName() + ".spec",
                    projectPath + "/" + newName + "/" + projectInfo->getName() + ".spec");
    }
}
