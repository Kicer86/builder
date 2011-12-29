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
#include <QDebug>
#include <QtPlugin>
#include <QGridLayout>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QProcess>
#include <QProgressBar>
#include <QTemporaryFile>
#include <QSettings>

#include <debug.hpp>

#include "rpmbuildplugin.hpp"
#include "data_containers/editorsmanager.hpp"
#include "data_containers/projectsmanager.hpp"
#include "data_containers/releaseinfo.hpp"
#include "data_containers/projectinfo.hpp"
#include "dialogs/specconstantsdialog.hpp"
#include "misc/functions.hpp"
#include "misc/sandboxprocess.hpp"
#include "misc/settings.hpp"


Q_EXPORT_PLUGIN2(RPMbuild_plugin, RpmBuildPlugin)

static const QString buildButtonText(QObject::tr("Build"));
static const QString fastBuildButtonText(QObject::tr("Fast build"));

RpmBuildPlugin::RpmBuildPlugin(): BuildPlugin("RPM builer"), buttonsEnabled(false)
{
    //construct layout
    buttons = new QGridLayout;
    log = new QPlainTextEdit;
    buildButton = new QPushButton(buildButtonText);
    fastBuildButton = new QPushButton(fastBuildButtonText);

    editSpecButton = new QPushButton(tr("Edit spec file"));
    showMacrosButton = new QPushButton(tr("Show Rpm macros"));
    showConstantsButton = new QPushButton(tr("Show spec's constants"));

    progressBar = new QProgressBar;

    //1st column - build buttons
    buttons->addWidget(buildButton, 0, 0);
    buttons->addWidget(fastBuildButton, 1, 0);

    //2nd column - spacer
    buttons->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 1);


    //3rd and 4th columns - editing
    buttons->addWidget(editSpecButton, 0, 2);
    buttons->addWidget(showMacrosButton, 0, 3);
    buttons->addWidget(showConstantsButton, 1, 2, 1, 2);

    //3rd row - progress bar
    buttons->addWidget(progressBar, 2, 0, 1, 4);

    connect(buildButton, SIGNAL(pressed()), this, SLOT(buildButtonPressed()));
    connect(fastBuildButton, SIGNAL(pressed()), this, SLOT(fastBuildButtonPressed()));

    connect(editSpecButton, SIGNAL(clicked()), this, SLOT(specButtonPressed()));
    connect(showConstantsButton, SIGNAL(clicked()), this, SLOT(specConstantsButtonPressed()));
    connect(showMacrosButton, SIGNAL(clicked()), this, SLOT(showMacrosButtonPressed()));

    //disable all buttons until any ReleaseInfo is chosen
    buildButton->setDisabled(true);
    fastBuildButton->setDisabled(true);
    editSpecButton->setDisabled(true);
    showConstantsButton->setDisabled(true);
    showMacrosButton->setDisabled(true);
}


RpmBuildPlugin::~RpmBuildPlugin()
{}


void RpmBuildPlugin::build(RpmBuildPlugin::Type buildType)
{
    //get release currently selected
    ReleaseInfo * const releaseInfo = ProjectsManager::instance()->getCurrentRelease();

    if (releaseInfo == 0)
        return;                     //no release ich choosen

    //get info about this release
    BuildProcess *buildProcess = findBuildProcess(releaseInfo);
    if (buildProcess)   //there is an info about build?
    {
        //if it's running then stop it
        if (buildProcess->getProcess()->state() == QProcess::Running) //running?
        {
            buildProcess->stop();
            return;
        }
    }

    //włącz progress bar
    updateProgress(-1);  //powinien migać czy coś

    QString homePath = getenv("HOME");
    if (Settings::instance()->getEnvType() == Settings::External)
        homePath = "/root";


    const ProjectInfo *projectInfo = releaseInfo->getProjectInfo();
    const QString releasePath = releaseInfo->getReleasePath();
    const ReleaseInfo::VersionList &localVersions = *releaseInfo->getLocalVersions();
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

    //not created yet?
    if (buildProcess == 0)
        buildProcess = new BuildProcess(releaseInfo);

    buildProcess->getProcess()->setWorkingDirectory(SandboxProcess::decoratePath(""));

    startBuildProcess("rpmbuild", args, buildProcess);

    releaseInfo->buildStarted();

    updateTab();
}


void RpmBuildPlugin::buildButtonPressed()
{
    debug(DebugLevel::Debug) << "build button pressed";
    build(Normal);

    buildButton->setText(tr("Stop"));
    fastBuildButton->setDisabled(true);
}


void RpmBuildPlugin::fastBuildButtonPressed()
{
    debug(DebugLevel::Debug) << "fast build button pressed";
    build(Fast);

    fastBuildButton->setText(tr("Stop"));
    buildButton->setDisabled(true);
}


void RpmBuildPlugin::newReleaseInfoSelected(ReleaseInfo *)
{
    updateTab();

    if (buttonsEnabled == false)   //first ReleaseInfo chosen, enable buttons
    {
        buildButton->setEnabled(true);
        fastBuildButton->setEnabled(true);
        editSpecButton->setEnabled(true);
        showConstantsButton->setEnabled(true);
        showMacrosButton->setEnabled(true);

        buttonsEnabled = true;
    }
}


void RpmBuildPlugin::stopBuildProcess(ReleaseInfo *r)
{
    BuildPlugin::stopBuildProcess(r);

    //restore buttons
    buildButton->setEnabled(true);
    buildButton->setText(buildButtonText);

    fastBuildButton->setEnabled(true);
    fastBuildButton->setText(fastBuildButtonText);
}


void RpmBuildPlugin::specButtonPressed()
{
    ReleaseInfo *const releaseInfo = ProjectsManager::instance()->getCurrentRelease();

    QFileInfo fileInfo(releaseInfo->getSpecFile());
    if (fileInfo.exists() == false || fileInfo.size() == 0) //spec jeszcze nie istnieje? użyj tamplate
    {
        qDebug() << QString("preparing template for spec file (%1 -> %2)")
                    .arg(Functions::dataPath("spec_template"), fileInfo.absoluteFilePath() );

        //przygotuj spec pod bieżący projekt
        QFile src(Functions::dataPath("spec_template"));
        QFile dst(fileInfo.absoluteFilePath());
        src.open(QIODevice::ReadOnly);
        dst.open(QIODevice::WriteOnly);

        QString projName = releaseInfo->getProjectInfo()->getName();

        while (src.atEnd() == false)
        {
            QString line = src.readLine();

            QRegExp name("Name:.*$");
            if (name.exactMatch(line))       //uzupełnij nazwę projektu
                line = QString("Name:           %1\n").arg(projName);

            QRegExp mainSource("Source0:.*$");
            if (mainSource.exactMatch(line)) //uzupełnij główne źródło
                line = QString("Source0:        __FILEURL_%1__\n").arg(projName);

            QRegExp version("%define         version.*$");
            if (version.exactMatch(line))    //wersja programu
                line = QString("%define         version __VERSION_%1__\n").arg(projName);

            dst.write(line.toUtf8());
        }

        src.close();
        dst.close();
    }

    EditorsManager::instance()->editFile(fileInfo.absoluteFilePath());
}


void RpmBuildPlugin::specConstantsButtonPressed()
{
    SpecConstantsDialog dialog;

    ReleaseInfo *const releaseInfo = ProjectsManager::instance()->getCurrentRelease();

    //restore dialog's dimenstion
    QSettings settings;
    settings.beginGroup("Projects");
    settings.beginGroup(releaseInfo->getProjectInfo()->getName());
    settings.beginGroup(releaseInfo->getName());

    QByteArray dialogData = settings.value("Constants dialog").toByteArray();
    if (dialogData.size() > 0)
        dialog.restoreGeometry(dialogData);

    foreach(ProjectVersion projectVersion, *releaseInfo->getLocalVersions())
    {
        const QString &name = projectVersion.getName();
        const QString url = QString("__FILEURL_%1__").arg(name);
        const QString version = QString("__VERSION_%1__").arg(name);
        const QString extension = QString("__EXTENSION_%1__").arg(name);

        dialog.addConstant(url, projectVersion.getPkgUrl().toString());
        dialog.addConstant(version, projectVersion.getVersion());
        dialog.addConstant(extension, projectVersion.getExtension());

        dialog.addSeparator();
    }

    dialog.exec();

    //save dialog dimension
    settings.setValue("Constants dialog", dialog.saveGeometry());
    settings.endGroup();
    settings.endGroup();
    settings.endGroup();
}


void RpmBuildPlugin::showMacrosButtonPressed()
{
    //zapisz makra do pliku tymczasowego
    QTemporaryFile file;
    QProcess rpm;

    file.setFileTemplate("macrosXXXXXX");
    file.open();
    SandboxProcess::runProcess("rpm", QStringList() << "--showrc", &rpm);
    rpm.waitForFinished(-1);
    file.write(rpm.readAll());
    file.close();

    QProcess editor;
    editor.start("kwrite", QStringList() << file.fileName());
    editor.waitForFinished(-1);
}


QLayout* RpmBuildPlugin::getBuildButtons() const
{
    return buttons;
}


QWidget* RpmBuildPlugin::getBuildLog() const
{
    return log;
}


void RpmBuildPlugin::updateTab()
{
    //rpm's build log has been chosen
    //display log of particular releaseinfo

    ReleaseInfo *currentRelease = ProjectsManager::instance()->getCurrentRelease();
    if (currentRelease)
    {
        BuildProcess *const buildProcess = findBuildProcess(currentRelease);
        if (buildProcess)
            log->setDocument(buildProcess->getLog());
        else
            log->setDocument(nullptr);
    }
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
