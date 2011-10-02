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

#include <QDebug>
#include <QSettings>
#include <QDir>

#include "data_containers/projectsmanager.hpp"
#include "data_containers/projectinfo.hpp"
#include "misc/downloader.hpp"
#include "misc/estimator.hpp"
#include "misc/sandboxprocess.hpp"
#include "misc/settings.hpp"
#include "releaseinfo.hpp"


ReleaseInfo::ReleaseInfo(const QString &n, ProjectInfo* p):
    QObject(0), id(ProjectsManager::instance()->getId()),
    name(n), total(100), done(0), projectInfo(p), state(Nothing)
{
//   buildProcess=new QProcess(this);
//   buildingLog=new QTextDocument(this);
  estimator=new Estimator(this);


  qDebug() << QString("adding release %1").arg(name);
  QSettings settings;
  settings.beginGroup("Projects");
  settings.beginGroup(projectInfo->getName());
  settings.beginGroup(name);
  download=settings.value("download").toBool();
  build=settings.value("build").toBool();

  //lua script
  downloadScript=releasePath() + "/download.lua";

  //read local version
  int size=settings.beginReadArray("localVersion");
  for (int i=0; i<size; i++)
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

//   connect(buildProcess, SIGNAL(readyRead()), this, SLOT(buildMessages()));
//   connect(buildProcess, SIGNAL(finished( int, QProcess::ExitStatus )),
//           this, SLOT(buildFinished(int, QProcess::ExitStatus)));
//   connect(buildProcess, SIGNAL(error(QProcess::ProcessError)),
//           this, SLOT(buildError(QProcess::ProcessError)));
}


ReleaseInfo::~ReleaseInfo()
{
  qDebug() << QString("deleting release %1").arg(name);
  QSettings settings;
  settings.beginGroup("Projects");
  settings.beginGroup(projectInfo->getName());
  settings.beginGroup(name);
  settings.setValue("download", download);
  settings.setValue("build", build);

  settings.beginWriteArray("localVersion");
  int i=0;
  foreach(ProjectVersion pV, localVersions)
  {
    QString pkgName=pV.getName();
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
  return QString("%1/%2/%3").arg(Settings::instance()->getProjectsPath())
         .arg(projectInfo->getName())
         .arg(name);
}


void ReleaseInfo::setBuildOption(int checkState)
{
  build=checkState==Qt::Checked;
  emit optionsChanged();
}


void ReleaseInfo::setDownloadOption(int checkState)
{
  download=checkState==Qt::Checked;
  emit optionsChanged();
}


// void ReleaseInfo::updateProgress(int d, int t)
// {
//   done=d;
//   total=t;
// 
//   emit statusChanged(ProgressChange);
// }


// void ReleaseInfo::updateProgress(qint64 d, qint64 t)
// {
//   done=d;
//   total=t;
// 
//   emit statusChanged(ProgressChange);
// }
// 
// 
void ReleaseInfo::setState(ReleaseInfo::State st)
{
  state=st;
//   if (state==Nothing)
//     updateProgress(0,100);
  
  //status has changed

  emit statusChanged(StateChange);
}


// void ReleaseInfo::appendTextToLog(const QString& msg)
// {
//   //dopisz text do logu
//   QTextCursor cursor(buildingLog);
//   cursor.movePosition(QTextCursor::End);
//   cursor.insertText(msg);
// 
//   if (state==Building)  //cmake daje %, użyjemy ich :]
//   {
//     QStringList lines=msg.split("\n");
//     foreach(QString line, lines)
//     {
//       QRegExp cmakeRegEx("^\\[([0-9 ]{3})\\%\\].*");
//       if (cmakeRegEx.exactMatch(line))
//         updateProgress(cmakeRegEx.capturedTexts()[1].toInt(), 100);
//     }
//   }
// }


// void ReleaseInfo::buildMessages()
// {
//   //emit signal
//   emit logWillChange();
//   
//   //append text to log
//   appendTextToLog(buildProcess->readAll()); //dopisz wszystko co zostało wyplute przez proces
// }


// void ReleaseInfo::buildFinished(int exitCode, QProcess::ExitStatus exitStatus)
// {
//   qDebug() << QString("Build process for %1:%2 finished with exit code: %3 and status: %4")
//   .arg(projectInfo->getName())
//   .arg(name)
//   .arg(exitCode)
//   .arg(exitStatus);
// 
//   appendTextToLog(tr("Process finished normally with exit code: %1 and status: %2")
//                   .arg(exitCode)
//                   .arg(exitStatus));
//   setState(Nothing);
// }


// void ReleaseInfo::buildError(QProcess::ProcessError error)
// {
//   qDebug() << QString("Build process for %1:%2 finished with error code: %3")
//   .arg(projectInfo->getName())
//   .arg(name)
//   .arg(error);
// 
//   appendTextToLog(tr("Process finished with error code %1").arg(error));
//   setState(Nothing);
// }


int ReleaseInfo::getId() const
{
  return id;
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


// QTextDocument* ReleaseInfo::getBuildMesages()
// {
//   return buildingLog;
// }


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
//   updateProgress(0,0);  //powinien migać czy coś

  if (! downloadScript.isEmpty())
  {
    setState(Checking);
    Downloader downloader;
    QFile script(downloadScript);
    if (script.exists() && script.open(QIODevice::ReadOnly))
    {
      currentVersions=downloader.checkVersion(script.readAll());
      script.close();
    }
    else
      qWarning() << QString("Could not open lua script file: %1").arg(downloadScript);

    foreach (ProjectVersion pV, currentVersions)
    qDebug() << "url to new version of" << projectInfo->getName() << "is" << pV.text() << pV.getExtension() << pV.getVersion();
  }
  setState(Nothing);
}


void ReleaseInfo::downloadPkg()
{
  if ( localVersions!=currentVersions && currentVersions.isEmpty()==false )
  {
    Downloader downloader;
    connect(&downloader, SIGNAL(progressUpdate(int,int)), this, SLOT(updateProgress(int,int)));
    connect(&downloader, SIGNAL(progressUpdate(qint64,qint64)), this, SLOT(updateProgress(qint64,qint64)));

    VersionList::iterator i;
    for (i=currentVersions.begin(); i!=currentVersions.end(); ++i)
    {
      ProjectVersion &pV=i.value();
      QString pkgName=pV.getName();

      if (localVersions.contains(pkgName)==false ||  //pobieraj dany plik tylko jesli różni się od wersji z dysku
          localVersions[pkgName].getVersion()!=pV.getVersion())
      {
        QFileInfo fileInfo(pV.getPkgUrl().toString()); //potrzebne do wyodrębnienia nazwy pliku z urlu sieciowego
        QFileInfo localFile(releasePath() + "/src/" + fileInfo.fileName()); //plik lokalny

        downloadedPkg=pkgName;   //ustaw nazwę pobieranej paczki
        setState(Downloading);   //zmien status (żeby interfejs się odświeżył)

        if (downloader.download(pV.getPkgUrl(), localFile.absoluteFilePath())==true)
        {
          pV.setLocalFile(localFile);
          localVersions[pkgName]=currentVersions[pkgName]; //synchronizuj dane
        }
        else
        {
          qWarning() << QString ("Could not download file: %1").arg(pV.getPkgUrl().toString());
          break;
        }
      }
    }
  }
  else
    qWarning() << "local and current versions are the same or current version not checked";

  setState(Nothing);
}

/*
void ReleaseInfo::buildPkg(BuildMode buildMode)
{

  if (buildProcess->state()!=QProcess::NotRunning) //chodzi?
  {
    //przerwij
    buildProcess->terminate();
    return;
  }

  //włącz progress bar
  updateProgress(0,0);  //powinien migać czy coś

  QString homePath=getenv("HOME");
  if (Settings::instance()->getEnvType()==Settings::External)
    homePath="/root";

  QString specFile= homePath + "/rpmbuild/SPECS/" + projectInfo->getName() + ".spec";
  QString specSrc=releasePath() + "/" + projectInfo->getName() + ".spec";
  QString specDst=SandboxProcess::decoratePath(specFile);

  qDebug() << QString("copying %1 to %2").arg(specSrc).arg(specDst);

  //skopiuj plik spec do SPECS (po uzupełnieniach)
  QFile src(specSrc);
  QFile dst(specDst);
  src.open(QIODevice::ReadOnly);
  dst.open(QIODevice::WriteOnly);

  while (src.atEnd()==false)
  {
    QString line=src.readLine();
    QRegExp version("(.*)__VERSION_([a-zA-Z0-9_-]+)__(.*)");
    if (version.exactMatch(line))
    {
      line=version.capturedTexts()[1];
      QString pkgName=version.capturedTexts()[2];
      if (localVersions.contains(pkgName))
        line+=localVersions[pkgName].getVersion();
      line+=version.capturedTexts()[3];
    }

    QRegExp extension("(.*)__EXTENSION_([a-zA-Z0-9_-]+)__(.*)");
    if (extension.exactMatch(line))
    {
      line=extension.capturedTexts()[1];
      QString pkgName=extension.capturedTexts()[2];
      if (localVersions.contains(pkgName))
        line+=localVersions[pkgName].getExtension();
      line+=extension.capturedTexts()[3];
    }

    QRegExp fileurl("(.*)__FILEURL_([a-zA-Z0-9_-]+)__(.*)");
    if (fileurl.exactMatch(line))
    {
      line=fileurl.capturedTexts()[1];
      QString pkgName=fileurl.capturedTexts()[2];
      if (localVersions.contains(pkgName))
        line+=localVersions[pkgName].getPkgUrl().toString();
      line+=fileurl.capturedTexts()[3];
    }

    dst.write(line.toUtf8());
  }

  src.close();
  dst.close();

  //skopiuj źródła
  foreach(ProjectVersion pV, localVersions)
  {
    qDebug() << QString("copying %1 to %2").arg(pV.getLocalFile().absoluteFilePath())
    .arg(SandboxProcess::decoratePath(homePath+"/rpmbuild/SOURCES/")+pV.getLocalFile().fileName());

    QFile::copy(pV.getLocalFile().absoluteFilePath(),
                SandboxProcess::decoratePath(
                  homePath+
                  "/rpmbuild/SOURCES/"+
                  pV.getLocalFile().fileName())
               );
  }

  //skopiuj patche
  QDir patchesDir(releasePath()+"/patches");
  patchesDir.cd(name);
  QStringList patches=patchesDir.entryList( QDir::Files, QDir::Name | QDir::IgnoreCase );

  foreach(QString patch, patches)
  {
    qDebug() << QString("copying %1 to %2").arg(releasePath()+"/patches/" + patch)
    .arg(SandboxProcess::decoratePath(homePath+"/rpmbuild/SOURCES/"+patch));

    QFile::copy(releasePath()+"/patches/" + patch,
                SandboxProcess::decoratePath(
                  homePath+
                  "/rpmbuild/SOURCES/"+
                  patch)
               );
  }

  buildingLog->clear();

  //zapuść maszynerię
  QStringList args;

  assert(buildMode==Normal || buildMode==Fast);

  if (buildMode==Normal)
    args << "-ba";
  else if (buildMode==Fast)
    args << "-bi" << "--short-circuit";

  args << specFile;

  buildProcess->setWorkingDirectory(SandboxProcess::decoratePath(""));
  buildProcess->setProcessChannelMode(QProcess::MergedChannels);

  QString infoMsg=SandboxProcess::runProcess("rpmbuild", args, buildProcess);
  qDebug() << QString ("Starting: %1").arg(infoMsg);
  appendTextToLog(tr("Starting: %1\n").arg(infoMsg));

  setState(Building);
}*/
