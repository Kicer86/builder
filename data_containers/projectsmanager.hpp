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


#ifndef PROJECTSMANAGER_HPP
#define PROJECTSMANAGER_HPP

#include <QList>
#include <QObject>

class QTimer;
class QStandardItemModel;

class ReleaseInfo;
class ProjectInfoWidget;
class ProjectInfo;


class ProjectsManager
{
    int id;                                 //id nadawane kolejnym projektom
    ProjectInfoWidget *projectInfoWidget;
    QList<ProjectInfo *> projectsList;      //lista projektów. Powiązana z modelem za pomocą id
    QStandardItemModel *model;              //model zawierający projekty

    ProjectsManager();

  public:
    virtual ~ProjectsManager();

    static ProjectsManager *instance();
    void registerProject(ProjectInfo *project);
    void destroyProjects();    
    void setProjectInfoWidget(ProjectInfoWidget *piW);
    ProjectInfo *findProject(int projectId);
    void showInfo(ReleaseInfo *releaseInfo);
    int getId();
    QStandardItemModel *getModel() const ;
    ReleaseInfo *getCurrentRelease() const;
};

#endif // PROJECTSMANAGER_HPP
