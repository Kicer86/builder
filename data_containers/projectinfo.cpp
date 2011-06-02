/*
    Klasa zawierająca informacje nt projektu
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

#include <QSettings>
#include <QDir>
#include <QDebug>
#include <QTextCursor>
#include <QTextDocument>
#include <QPlainTextEdit>

#include "misc/settings.hpp"
#include "misc/downloader.hpp"
#include "data_containers/projectsmanager.hpp"
#include "misc/sandboxprocess.hpp"
#include "misc/estimator.hpp"

#include "projectinfo.hpp"


ProjectInfo::ProjectInfo(QString n):id(ProjectsManager::instance()->getId()), name(n)
{
  qDebug() << QString("crating project %1 with id %2").arg(name).arg(id);
  QSettings setting;

  QDir releaseDir(Settings::instance()->getProjectsPath());
  releaseDir.cd(name);
  QStringList releases=releaseDir.entryList( QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name | QDir::IgnoreCase );

  foreach(QString release, releases)
  {
    ReleaseInfo *releaseInfo=new ReleaseInfo(release, this);
    releasesList.append(releaseInfo);
  }
}


ProjectInfo::~ProjectInfo()
{
  qDebug() << QString("destroying project %1 with %2 releases").arg(name).arg(releasesList.size());
  QSettings settings;
  while (releasesList.size()>0)
    delete releasesList.takeFirst();
}


const QList<ReleaseInfo*> *ProjectInfo::getReleasesList() const
{
  return &releasesList;
}


QString ProjectInfo::getName() const
{
  return name;
}


int ProjectInfo::getId() const
{
  return id;
}


void ProjectInfo::updateStatus() const
{
  Status st=Nothing;
  //przeleć releasy i sprawdź czy są jakieś do pobrania/budowania
  foreach(ReleaseInfo *ri, releasesList)
  {
    bool dwl=ri->getDownloadFlag();
    bool bld=ri->getBuildFlag();
   
    if (dwl && st<Check)
      st=Check;
    
    if (bld && st<Build)
      st=Build;
  }
  
  status=st;
}


ProjectInfo::Status ProjectInfo::getStatus() const
{
  return status;
}
