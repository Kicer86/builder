/*
    Rpm building plugin
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

#include <QDir>
#include <QtPlugin>
#include <QGridLayout>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QProcess>
#include <QProgressBar>
#include <QDebug>

#include <debug.hpp>

#include "rpmbuildplugin.hpp"
#include "data_containers/projectsmanager.hpp"
#include "data_containers/releaseinfo.hpp"
#include "data_containers/projectinfo.hpp"
#include "misc/sandboxprocess.hpp"
#include "misc/settings.hpp"


Q_EXPORT_PLUGIN2(RPMbuild_plugin, RpmBuildPlugin)

RpmBuildPlugin::RpmBuildPlugin(): BuildPlugin("RPM builer")
{
    //construct layout
    buttons = new QGridLayout;
    log = new QPlainTextEdit;
    buildButton = new QPushButton(tr("Build"));
    fastBuildButton = new QPushButton(tr("Fast build"));
    progressBar = new QProgressBar;

    buttons->addWidget(buildButton, 0, 0);
    buttons->addWidget(fastBuildButton, 0, 1);
    buttons->addWidget(progressBar, 1, 0, 1, 2);

    connect(buildButton, SIGNAL(pressed()), this, SLOT(buildButtonPressed()));
    connect(fastBuildButton, SIGNAL(pressed()), this, SLOT(fastBuildButtonPressed()));
}


RpmBuildPlugin::~RpmBuildPlugin()
{}


void RpmBuildPlugin::build(RpmBuildPlugin::Type buildType)
{
    //get release currently selected
    ReleaseInfo * const releaseInfo = ProjectsManager::instance()->getCurrentRelease();
    const ProjectInfo *projectInfo = releaseInfo->getProjectInfo();
    const QString releasePath = releaseInfo->getReleasePath();
    const ReleaseInfo::VersionList &localVersions = *releaseInfo->getLocalVersions();

    if (releaseInfo == 0)
        return;                     //no release ich choosen

    //get info about this release
    BuildProcess *buildProcess = findBuildProcess(releaseInfo);
    if (buildProcess)   //there is an info about build? then stop it
    {
        buildProcess->stop();
        return;
    }

    //włącz progress bar
    updateProgress(-1);  //powinien migać czy coś

    QString homePath = getenv("HOME");
    if (Settings::instance()->getEnvType() == Settings::External)
        homePath = "/root";

    const QString specFile = homePath + "/rpmbuild/SPECS/" + projectInfo->getName() + ".spec";
    const QString specSrc = releasePath + "/" + projectInfo->getName() + ".spec";
    const QString specDst = SandboxProcess::decoratePath(specFile);

    debug(DebugLevel::Info) << QString("copying %1 to %2").arg(specSrc).arg(specDst);

    //skopiuj plik spec do SPECS (po uzupełnieniach)
    QFile src(specSrc);
    QFile dst(specDst);
    src.open(QIODevice::ReadOnly);
    dst.open(QIODevice::WriteOnly);

    while (src.atEnd() == false)
    {
        QString line = src.readLine();
        const QRegExp version("(.*)__VERSION_([a-zA-Z0-9_-]+)__(.*)");

        if (version.exactMatch(line))
        {
            line = version.capturedTexts()[1];
            const QString pkgName = version.capturedTexts()[2];
            if (releaseInfo->getLocalVersions()->contains(pkgName))
                line += localVersions[pkgName].getVersion();
            line += version.capturedTexts()[3];
        }

        const QRegExp extension("(.*)__EXTENSION_([a-zA-Z0-9_-]+)__(.*)");
        if (extension.exactMatch(line))
        {
            line = extension.capturedTexts()[1];
            const QString pkgName = extension.capturedTexts()[2];
            if (localVersions.contains(pkgName))
                line += localVersions[pkgName].getExtension();
            line += extension.capturedTexts()[3];
        }

        const QRegExp fileurl("(.*)__FILEURL_([a-zA-Z0-9_-]+)__(.*)");
        if (fileurl.exactMatch(line))
        {
            line = fileurl.capturedTexts()[1];
            const QString pkgName = fileurl.capturedTexts()[2];
            if (localVersions.contains(pkgName))
                line += localVersions[pkgName].getPkgUrl().toString();
            line += fileurl.capturedTexts()[3];
        }

        dst.write(line.toUtf8());
    }

    src.close();
    dst.close();

    //skopiuj źródła
    foreach(ProjectVersion pV, localVersions)
    {
        debug(DebugLevel::Info) << QString("copying %1 to %2").arg(pV.getLocalFile().absoluteFilePath())
        .arg(SandboxProcess::decoratePath(homePath + "/rpmbuild/SOURCES/") + pV.getLocalFile().fileName());

        QFile::copy(pV.getLocalFile().absoluteFilePath(),
                    SandboxProcess::decoratePath(
                        homePath +
                        "/rpmbuild/SOURCES/" +
                        pV.getLocalFile().fileName())
                   );
    }

    //skopiuj patche
    QDir patchesDir(releasePath + "/patches");
    patchesDir.cd(releaseInfo->getName());
    const QStringList patches = patchesDir.entryList( QDir::Files, QDir::Name | QDir::IgnoreCase );

    foreach(QString patch, patches)
    {
        debug(DebugLevel::Info) << QString("copying %1 to %2")
                                   .arg(releasePath + "/patches/" + patch)
                                   .arg(SandboxProcess::decoratePath(homePath + "/rpmbuild/SOURCES/" + patch));

        QFile::copy(releasePath + "/patches/" + patch,
                    SandboxProcess::decoratePath(
                        homePath +
                        "/rpmbuild/SOURCES/" +
                        patch)
                   );
    }

    //zapuść maszynerię
    QStringList args;

    assert(buildType == Normal || buildType == Fast);

    if (buildType == Normal)
        args << "-ba";
    else if (buildType == Fast)
        args << "-bi" << "--short-circuit";

    args << specFile;

    buildProcess = new BuildProcess(releaseInfo);
    buildProcess->getProcess()->setWorkingDirectory(SandboxProcess::decoratePath(""));

    addBuildProcess("rpmbuild", args, buildProcess);

//   setState(Building);
}


void RpmBuildPlugin::buildButtonPressed()
{
    debug(DebugLevel::Debug) << "build button pressed";
    build(Normal);
}


void RpmBuildPlugin::fastBuildButtonPressed()
{
    debug(DebugLevel::Debug) << "fast build button pressed";
    build(Fast);
}


QLayout* RpmBuildPlugin::getBuildButtons() const
{
    return buttons;
}


QWidget* RpmBuildPlugin::getBuildLog() const
{
    return log;
}


void RpmBuildPlugin::updateProgress(int progress)
{
    if (progress == -1)
        progressBar->setRange(0, 0);
    else
    {
        progressBar->setMaximum(100);
        progressBar->setValue(progress);
    }
}
