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

#include <QSettings>
#include <QDir>

#include <debug.hpp>

#include "data_containers/projectsmanager.hpp"
#include "data_containers/projectinfo.hpp"
#include "misc/downloader.hpp"
#include "misc/estimator.hpp"
#include "misc/sandboxprocess.hpp"
#include "misc/settings.hpp"
#include "releaseinfo.hpp"


ReleaseInfo::ReleaseInfo(const QString &n, ProjectInfo* p):
        QObject(0),
        name(n), builds(0), total(100), done(0), projectInfo(p),
        state(State::Nothing)
{
    estimator = new Estimator(this);

    debug(DebugLevel::Debug) << QString("adding release %1").arg(name);
    QSettings settings;
    settings.beginGroup("Projects");
    settings.beginGroup(projectInfo->getName());
    settings.beginGroup(name);
    download = settings.value("download").toBool();
    build = settings.value("build").toBool();

    //lua script
    downloadScript = releasePath() + "/download.lua";

    //read local version
    const int size = settings.beginReadArray("localVersion");
    for (int i = 0; i < size; i++)
    {
        settings.setArrayIndex(i);
        ProjectVersion pV;
        pV.load(&settings);
        localVersions[pV.getName()] = pV;
    }
    settings.endArray();

    settings.endGroup();
    settings.endGroup();
    settings.endGroup();
}


ReleaseInfo::~ReleaseInfo()
{
    debug(DebugLevel::Debug) << QString("deleting release %1").arg(name);
    QSettings settings;
    settings.beginGroup("Projects");
    settings.beginGroup(projectInfo->getName());
    settings.beginGroup(name);
    settings.setValue("download", download);
    settings.setValue("build", build);

    settings.beginWriteArray("localVersion");
    int i = 0;
    for(const ProjectVersion &pV: localVersions)
    {
        QString pkgName = pV.getName();
        settings.setArrayIndex(i);
        if (localVersions[pkgName].save(&settings))
            i++;            //inkrementuj tylko jesli było coś do zapisania
    }
    settings.endArray();

    settings.endGroup();
    settings.endGroup();
    settings.endGroup();
}


QString ReleaseInfo::releasePath() const
{
    return QString("%1/%2").arg(projectInfo->getPath())
                           .arg(name);
}


void ReleaseInfo::setBuildOption(int checkState)
{
    build = checkState == Qt::Checked;
    emit optionsChanged();
}


void ReleaseInfo::setDownloadOption(int checkState)
{
    download = checkState == Qt::Checked;
    emit optionsChanged();
}


void ReleaseInfo::updateProgress(int d, int t)
{
    done = d;
    total = t;

    emit statusChanged( ChangeType::ProgressChange );
}


void ReleaseInfo::updateProgress(qint64 d, qint64 t)
{
    done = d;
    total = t;

    emit statusChanged(ChangeType::ProgressChange);
}


void ReleaseInfo::setState(ReleaseInfo::State st)
{
    state = st;

    //status has changed
    emit statusChanged(ChangeType::StateChange);
}


QString ReleaseInfo::getName() const
{
    return name;
}


bool ReleaseInfo::getDownloadFlag() const
{
    return download;
}


bool ReleaseInfo::getBuildFlag() const
{
    return build;
}


QString ReleaseInfo::getDownloadScriptFile() const
{
    return downloadScript;
}


QString ReleaseInfo::getSpecFile() const
{
    return releasePath() + "/" + projectInfo->getName() + ".spec";
}


QString ReleaseInfo::getReleasePath() const
{
    return releasePath();
}


const ProjectInfo* ReleaseInfo::getProjectInfo() const
{
    return projectInfo;
}


const QString& ReleaseInfo::getDownloadedPkg() const
{
    return downloadedPkg;
}


const ReleaseInfo::VersionList* ReleaseInfo::getLocalVersions() const
{
    return &localVersions;
}


const ReleaseInfo::VersionList* ReleaseInfo::getCurrentVersions() const
{
    return &currentVersions;
}


ReleaseInfo::State ReleaseInfo::getState() const
{
    return state;
}


qint64 ReleaseInfo::getProgressDone() const
{
    return done;
}


qint64 ReleaseInfo::getProgressTotal() const
{
    return total;
}


const Estimator* ReleaseInfo::getEstimator() const
{
    return estimator;
}


void ReleaseInfo::update()
{
    //włącz progress bar
    updateProgress(0, 0); //powinien migać czy coś

    if ( downloadScript.isEmpty() == false )
    {
        setState(State::Checking);
        Downloader downloader;
        QFile script(downloadScript);
        if (script.exists() && script.open(QIODevice::ReadOnly))
        {
            currentVersions = downloader.checkVersion(script.readAll());
            script.close();
        }
        else
            debug(DebugLevel::Warning) << QString("Could not open lua script file: %1").arg(downloadScript);

        for(const ProjectVersion &pV: currentVersions)
            debug(DebugLevel::Info) << "url to new version of " << projectInfo->getName()
                                    << " is " << pV.text()
                                    << " " << pV.getExtension() << " " << pV.getVersion();
    }
    setState(State::Nothing);
}


void ReleaseInfo::downloadPkg()
{
    if ( localVersions != currentVersions && currentVersions.isEmpty() == false )
    {
        Downloader downloader;
        connect(&downloader, SIGNAL(progressUpdate(int, int)), this, SLOT(updateProgress(int, int)));
        connect(&downloader, SIGNAL(progressUpdate(qint64, qint64)), this, SLOT(updateProgress(qint64, qint64)));

        for (ProjectVersion &remoteVersion: currentVersions)
        {
            const QString pkgName = remoteVersion.getName();

            if ( remoteVersion.getStatus() == ProjectVersion::Status::Filled &&  //there is any valid data?
                (localVersions.contains(pkgName) == false ||                          //local version does not contains this file?
                 localVersions[pkgName].getVersion() != remoteVersion.getVersion())   //or versions are different?
               )
            {
                QFileInfo fileInfo(remoteVersion.getPkgUrl().toString()); //potrzebne do wyodrębnienia nazwy pliku z urlu sieciowego
                QFileInfo localFile(releasePath() + "/src/" + fileInfo.fileName()); //plik lokalny

                downloadedPkg = pkgName;       //ustaw nazwę pobieranej paczki
                setState(State::Downloading);  //zmien status (żeby interfejs się odświeżył)

                if (downloader.download(remoteVersion.getPkgUrl(), localFile.absoluteFilePath()) == true)
                {
                    remoteVersion.setLocalFile(localFile);
                    localVersions[pkgName] = currentVersions[pkgName]; //synchronizuj dane
                }
                else
                {
                    debug(DebugLevel::Warning) << QString ("Could not download file: %1").arg(remoteVersion.getPkgUrl().toString());
                    break;
                }
            }
        }
    }
    else
        debug(DebugLevel::Warning) << "local and current versions are the same or current version not checked";

    setState(State::Nothing);
}


void ReleaseInfo::buildStarted()
{
    if (builds++ == 0)
        setState(State::Building);  //first build process? set state to building
}


void ReleaseInfo::buildStopped()
{
    if (--builds == 0)
        setState(State::Nothing);   //last build process finished? set state to nothing
}
