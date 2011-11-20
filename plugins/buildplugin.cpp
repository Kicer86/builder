/*
    Interface for builing plugins
    Copyright (C) 2011  Michał Walenciak <Kicer86@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <assert.h>

#include <QPlainTextDocumentLayout>
#include <QTextCursor>
#include <QProcess>
#include <QTextDocument>

#include <debug.hpp>

#include "buildplugin.hpp"
#include "data_containers/releaseinfo.hpp"
#include "data_containers/projectinfo.hpp"
#include "misc/sandboxprocess.hpp"


BuildProcess::BuildProcess(): process(0)
{
    log=new QTextDocument(this);
    process=new QProcess(this);
    process->setProcessChannelMode(QProcess::MergedChannels);

    QPlainTextDocumentLayout *documentLayout=new QPlainTextDocumentLayout(log);
    log->setDocumentLayout(documentLayout);

    connect(process, SIGNAL(readyRead()), this, SLOT(read()));
    connect(process, SIGNAL(terminated()), this, SLOT(close()));
}


BuildProcess::~BuildProcess()
{}


void BuildProcess::appendToLog(const QString &str) const
{
//append text to logu
    QTextCursor cursor(log);
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(str);
}


void BuildProcess::read() const
{
    appendToLog(process->readAll());


    /*
    if (state==Building)  //cmake daje %, użyjemy ich :]
    {
      QStringList lines=msg.split("\n");
      foreach(QString line, lines)
      {
        QRegExp cmakeRegEx("^\\[([0-9 ]{3})\\%\\].*");
        if (cmakeRegEx.exactMatch(line))
          updateProgress(cmakeRegEx.capturedTexts()[1].toInt(), 100);
      }
    }
    */
}


void BuildProcess::close() const
{
    emit removeBuildProcess(releaseInfo);
}


void BuildProcess::stop() const
{
    process->terminate();
}


BuildPlugin::BuildPlugin(const char *n): name(n)
{
    debug(DebugLevel::Debug) << "Loading plugin " << name;
}


BuildPlugin::~BuildPlugin()
{
    debug(DebugLevel::Debug) << "Unloading plugin " << name;
}


QString BuildPlugin::getBuilderName() const
{
    return name;
}


void BuildPlugin::addBuildProcess(const QString &program, const QStringList &args, BuildProcess* buildProcess)
{
    //make sure there is no such process already in base
    assert(findBuildProcess(buildProcess->releaseInfo)==0);

    debug(DebugLevel::Info) << "Starting new build process for project "
    << buildProcess->releaseInfo->getProjectInfo()->getName()
    << ", release: "
    << buildProcess->releaseInfo->getName();

    connect(buildProcess, SIGNAL(removeBuildProcess(ReleaseInfo*)), this, SLOT(removeBuildProcess(ReleaseInfo*)));

    buildsInfo[buildProcess->releaseInfo]=buildProcess;
//   buildProcess->process->start();
    QString infoMsg=SandboxProcess::runProcess(program, args, buildProcess->process);

    debug(DebugLevel::Info) << QString ("Starting: %1").arg(infoMsg);
    buildProcess->appendToLog(tr("Starting: %1\n").arg(infoMsg));
}


BuildProcess* BuildPlugin::findBuildProcess(ReleaseInfo *releaseInfo)
{
    return *(buildsInfo.find(releaseInfo));
}


void BuildPlugin::removeBuildProcess(ReleaseInfo *releaseInfo)
{
    BuildProcess *buildProcess=findBuildProcess(releaseInfo);
    if (buildProcess)
    {
        assert(buildProcess->process->state()==QProcess::NotRunning);  //process should be halted

        //remove all data
        delete buildsInfo.take(buildProcess->releaseInfo);
        delete buildProcess;
    }
}
