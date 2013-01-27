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

#include <OpenLibrary/common/debug.hpp>

#include "buildplugin.hpp"
#include "data_containers/releaseinfo.hpp"
#include "data_containers/projectinfo.hpp"
#include "misc/broadcast.hpp"
#include "misc/sandboxprocess.hpp"


BuildProcess::BuildProcess(ReleaseInfo *r): releaseInfo(r), data(0)
{
    log = new QTextDocument(this);
    process = new QProcess(this);
    process->setProcessChannelMode(QProcess::MergedChannels);

    QPlainTextDocumentLayout *documentLayout=new QPlainTextDocumentLayout(log);
    log->setDocumentLayout(documentLayout);

    connect(process, SIGNAL(started()), this, SLOT(started()));
    connect(process, SIGNAL(readyRead()), this, SLOT(read()));
    connect(process, SIGNAL(finished(int)), this, SLOT(close(int)));
}


BuildProcess::~BuildProcess()
{}


void BuildProcess::appendToLog(const QString &str) const
{
    //append text to log
    QTextCursor cursor(log);
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(str);
}


void BuildProcess::started()
{
    progressBar.setRange(0, 0);
}


void BuildProcess::read() const
{
    const QByteArray msg = process->readAll();
    appendToLog(msg);

    /*
    if (state == Building)  //cmake daje %, użyjemy ich :]
    {
        QStringList lines = msg.split("\n");
        foreach(QString line, lines)
        {
            QRegExp cmakeRegEx("^\\[([0-9 ]{3})\\%\\].*");
            if (cmakeRegEx.exactMatch(line))
                updateProgress(cmakeRegEx.capturedTexts()[1].toInt(), 100);
        }
    }
    */
}


void BuildProcess::close(int)
{
    progressBar.setRange(0, 100);
    progressBar.setValue(0);
    emit buildProcessStopped(releaseInfo);
}


void BuildProcess::stop() const
{
    process->terminate();
}


bool BuildProcess::isRunning() const
{
    return process->state() != QProcess::NotRunning;
}


BuildPlugin::BuildPlugin(const char *n): currentReleaseInfo(nullptr), pluginName(n)
{
    debug(DebugLevel::Debug) << "Loading plugin " << pluginName;

    //watch for changes in current (selected) ReleaseInfo
    connect(Broadcast::instance(), SIGNAL(releaseSelectedSignal(ReleaseInfo *)), this, SLOT(releaseInfoSelected(ReleaseInfo *)));
}


BuildPlugin::~BuildPlugin()
{
    debug(DebugLevel::Debug) << "Unloading plugin " << pluginName;
}


const QString& BuildPlugin::getBuilderName() const
{
    return pluginName;
}


void BuildPlugin::startBuildProcess(const QString &program, const QStringList &args, BuildProcess* buildProcess)
{
    debug(DebugLevel::Info) << "Starting new build process for project "
                            << buildProcess->releaseInfo->getProjectInfo()->getName()
                            << ", release: "
                            << buildProcess->releaseInfo->getName();

    //make sure there is no such process already in base
    if (findBuildProcess(buildProcess->releaseInfo) == nullptr)
    {
        connect(buildProcess, SIGNAL(buildProcessStopped(ReleaseInfo*)), this, SLOT(stopBuildProcess(ReleaseInfo*)));

        buildsInfo[buildProcess->releaseInfo] = buildProcess;
    }
    else
        buildProcess->getLog()->clear();  //we start new build, clear log

    QString infoMsg = SandboxProcess::runProcess(program, args, buildProcess->process);

    debug(DebugLevel::Info) << QString ("Starting: %1").arg(infoMsg);
    buildProcess->appendToLog(tr("Starting: %1\n").arg(infoMsg));
}


BuildProcess* BuildPlugin::findBuildProcess(const ReleaseInfo *releaseInfo)
{
    BuildsInfo::iterator it = buildsInfo.find(releaseInfo);

    return (it != buildsInfo.end()) ? (it->second) : 0;
}


void BuildPlugin::stopBuildProcess(ReleaseInfo *releaseInfo)
{
    BuildProcess *buildProcess = findBuildProcess(releaseInfo);
    assert(buildProcess);

    if (buildProcess)
    {
        assert(buildProcess->isRunning() == false);  //process should be halted

        //remove all data
        BuildsInfo::iterator it = buildsInfo.find(releaseInfo);
        assert (it != buildsInfo.end());
        //buildsInfo.erase(it);

        //buildProcess->deleteLater();

        releaseInfo->buildStopped();
        buildProcessStopped(releaseInfo);
    }
}


void BuildPlugin::releaseInfoSelected(ReleaseInfo *r)
{
    currentReleaseInfo = r;            //save current ReleaseInfo

    newReleaseInfoSelected();          //call virtual protected function
}
