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
#include "data_containers/releaseinfo.hpp"
#include "data_containers/projectinfo.hpp"
#include "dialogs/specconstantsdialog.hpp"
#include "misc/functions.hpp"
#include "misc/sandboxprocess.hpp"
#include "misc/settings.hpp"


Q_EXPORT_PLUGIN2(RPMbuild_plugin, RpmBuildPlugin)

static const QString buildButtonText(QObject::tr("Build"));
static const QString fastBuildButtonText(QObject::tr("Fast build"));

static void append(QHash<QString, QString> &hash, const QList< std::pair<QString, QString> > &list)
{
    for(const std::pair<QString, QString> &element: list)
        hash[element.first] = element.second;
}

static void append(QHash<QString, QString> &hash, const QHash<QString, QString> &other)
{
    for (auto i = other.begin(); i != other.end(); ++i)
        hash.insert(i.key(), i.value());
}


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


QString RpmBuildPlugin::replaceVariables(const QString &str, const Hash &variables) const
{
    const int size = str.size();
    QString result;

    for (int i = 0; i < size; i++)
    {
        QString toAppend(str[i]);

        if (i < (size - sizeof("__a__")))  // enought chars to create shortest variable?
        {
            if (str[i] == '_' && str[i + 1] == '_')  //prefix matches? (__)
            {
                const int j = i + 2;
                i = j;                               //jump over prefix
                toAppend += '_';

                while ( (i + 1) < size && (str[i] != '_' || str[i + 1] != '_') )  //wait for suffix (__)
                    toAppend += str[i++];

                if (i + 1 < size && str[i] == '_' && str[i + 1] == '_')
                {
                    toAppend += "__";
                    i++;

                    //we got it!
                    const int k = i - 2;  //name without suffix

                    const QString varName = str.mid(j, k - j + 1);

                    if (variables.contains(varName))
                    {
                        toAppend = variables[varName];
                        debug(DebugLevel::Info) << "Variable \"" << varName << "\" used in spec file. (value = \"" << variables[varName] << "\")";
                    }
                    else
                        debug(DebugLevel::Warning) << "Unknown variable \"" << varName << "\" used in spec file.";
                }
            }
        }

        result.append(toAppend);
    }

    return result;
}


RpmBuildPlugin::List RpmBuildPlugin::getListOfConstants(const ReleaseInfo *releaseInfo) const
{
    RpmBuildPlugin::List list;

    for(const ProjectVersion &projectVersion: *releaseInfo->getLocalVersions())
    {
        const QString &name = projectVersion.getName();
        const QString url = QString("FILEURL_%1").arg(name);
        const QString version = QString("VERSION_%1").arg(name);
        const QString extension = QString("EXTENSION_%1").arg(name);

        list.push_back( Pair(url, projectVersion.getPkgUrl().toString()) );
        list.push_back( Pair(version, projectVersion.getVersion()) );
        list.push_back( Pair(extension, projectVersion.getExtension()) );
    }

    return list;
}


RpmBuildPlugin::List RpmBuildPlugin::getListOfVariables(const ReleaseInfo *releaseInfo) const
{
    RpmBuildPlugin::List list;

    QSettings settings;
    settings.beginGroup("Projects");
    settings.beginGroup(releaseInfo->getProjectInfo()->getName());
    settings.beginGroup(releaseInfo->getName());
    settings.beginGroup("spec variables");

    for (const QString &key: settings.childKeys())
    {
        const QString value = settings.value(key).toString();
        list.push_back( Pair(key, value) );
    }

    settings.endGroup();
    settings.endGroup();
    settings.endGroup();
    settings.endGroup();

    return list;
}


RpmBuildPlugin::Hash RpmBuildPlugin::solveVariables(const List &variables,         //variables to solve
                                                    const List &constants          //constants (solved)
                                                   ) const
{
    Hash hash;

    append(hash, constants);    //fill list of solved constants/variables with constants

    std::function<bool(const Pair &var)> resolveVariable;

    resolveVariable = [&](const Pair &var) -> bool
    {

        auto error = [&](const QString &variable, const QString &value, const QString &message)
        {
            debug(DebugLevel::Error) << "Could not resolve variable \"" << variable << "\""
                                     << "with value \"" << value << "\""
                                     << ". Error message: " << message;
        };

        assert(hash.contains(var.first) == false);

        //check first char of value
        if (var.second.size() == 0)
            hash[var.first] = var.second;
        else
        {
            const char operation = var.second[0].toAscii() ;

            switch (operation)
            {
                case '=':       //just use variable's value
                    hash[var.first] = replaceVariables(var.second.mid(1), hash);   //do not use first char which is '='
                    break;

                case 'r':       //regexp
                {
                    //Format for this operation:
                    //  "r:expression:regexp:result"
                    // ie: "r:abc:a(.*):_%1_" will return "_bc_".  (%1 is equivalent of \1)
                    // This operation is +- equivalent of:
                    // echo $expression | sed -e "s/$regexp/$result/"
                    // Instead of ":" as separator, any other char may be used.

                    const QString value = var.second.mid(1);   //all chars except 1. char
                    const size_t size = value.size();
                    if (size >= sizeof(":::"))                 //minimal regexp is: ":::"
                    {
                        const QString solvedValue = replaceVariables(value, hash);  //replace all variables with theirs values
                        const char splitter = solvedValue[0].toAscii();  //use first char as splitter

                        //do splitting
                        const QStringList regexpParts = solvedValue.mid(1).split(splitter);  //ommit first ':'

                        //we are expecting 3 parts
                        if (regexpParts.size() == 3)
                        {
                            QRegExp regexp(regexpParts[1]);        //second part is a pattern

                            if (regexp.exactMatch(regexpParts[0])) //enter string to be matched (first part)
                            {
                                //now try to commit matched expression to result
                                const QStringList captured = regexp.capturedTexts();  //list of captured texts "(.*)"
                                QString result = regexpParts[2];

                                for (int i = 1; i < captured.size(); i++)
                                {
                                    //this operation will replace each "%n" in result part of regexp with corresponding captured text.
                                    //This is why we use %n instead of \n, as %n is used by QString algorithms to replace args
                                    result = result.arg(captured[i]);
                                }

                                hash[var.first] = result;
                            }
                            else
                                error(var.first, var.second, "Could not match regular expression.");
                        }
                        else
                            error(var.first, var.second,
                                  QString("Variable's value should consist of 3 parts, but %1 were found.")
                                  .arg(regexpParts.size()));
                    }
                    else
                        error(var.first, var.second, "Variable's value is too short even for simples regular expression.");
                }
                break;

                default:
                    debug(DebugLevel::Error) << "unknown variable operator: \"" << operation << "\"";
                    break;
            }
        }

        return true;
    };

    for (const Pair &var: variables)
    {
        //try to resolve value

        if (hash.contains(var.first) == false)  //not resolved yet?
            resolveVariable(var);
    }

    return hash;
}


void RpmBuildPlugin::refreshGui()
{
    //now get it's build process
    if (currentReleaseInfo != nullptr)
    {
        if (buttonsEnabled == false)   //first ReleaseInfo chosen, enable buttons
        {
            buildButton->setEnabled(true);
            fastBuildButton->setEnabled(true);
            editSpecButton->setEnabled(true);
            showConstantsButton->setEnabled(true);
            showMacrosButton->setEnabled(true);

            buttonsEnabled = true;
        }

        const BuildProcess *buildProcess = findBuildProcess(currentReleaseInfo);

        if (buildProcess != nullptr && buildProcess->isRunning())
        {
            switch (buildProcess->getData())
            {
                case Normal:
                    buildButton->setText(tr("Stop"));
                    fastBuildButton->setDisabled(true);
                    break;

                case Fast:
                    fastBuildButton->setText(tr("Stop"));
                    buildButton->setDisabled(true);
                    break;

                default:
                    assert(!"bug, unregonized build type");
                    break;
            }
        }
        else
        {
            //process is not running, restore buttons
            buildButton->setEnabled(true);
            buildButton->setText(buildButtonText);

            fastBuildButton->setEnabled(true);
            fastBuildButton->setText(fastBuildButtonText);
        }
    }

}


void RpmBuildPlugin::build(RpmBuildPlugin::Type buildType)
{
    if (currentReleaseInfo == 0)
        return;                     //no release ich choosen

    //get info about this release
    BuildProcess *buildProcess = findBuildProcess(currentReleaseInfo);
    if (buildProcess)   //there is an info about build?
    {
        //if it's running then stop it
        if (buildProcess->isRunning()) //running?
        {
            buildProcess->stop();
            return;
        }
    }

    QString homePath = getenv("HOME");
    if (Settings::instance()->getEnvType() == Settings::External)
        homePath = "/root";


    const ProjectInfo *projectInfo = currentReleaseInfo->getProjectInfo();
    const QString releasePath = currentReleaseInfo->getReleasePath();
    const ReleaseInfo::VersionList &localVersions = *currentReleaseInfo->getLocalVersions();
    const QString specFile = homePath + "/rpmbuild/SPECS/" + projectInfo->getName() + ".spec";
    const QString specSrc = releasePath + "/" + projectInfo->getName() + ".spec";
    const QString specDst = SandboxProcess::decoratePath(specFile);

    debug(DebugLevel::Info) << QString("copying %1 to %2").arg(specSrc).arg(specDst);

    //skopiuj plik spec do SPECS (po uzupełnieniach)
    QFile src(specSrc);
    QFile dst(specDst);
    src.open(QIODevice::ReadOnly);
    dst.open(QIODevice::WriteOnly);

    //prepare hash of constants and variables
    QHash<QString, QString> list;
    List constants = getListOfConstants(currentReleaseInfo);
    List variables = getListOfVariables(currentReleaseInfo);
    append(list, constants);
    append(list, solveVariables(variables, constants));

    while (src.atEnd() == false)
    {
        const QString line = replaceVariables( src.readLine(), list );
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
    patchesDir.cd(currentReleaseInfo->getName());
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
        buildProcess = new BuildProcess(currentReleaseInfo);

    buildProcess->setData(buildType);
    buildProcess->getProcess()->setWorkingDirectory(SandboxProcess::decoratePath(""));

    startBuildProcess("rpmbuild", args, buildProcess);

    currentReleaseInfo->buildStarted();

    updateTab();
}


void RpmBuildPlugin::buildButtonPressed()
{
    debug(DebugLevel::Debug) << "build button pressed";
    build(Normal);

    refreshGui();
}


void RpmBuildPlugin::fastBuildButtonPressed()
{
    debug(DebugLevel::Debug) << "fast build button pressed";
    build(Fast);

    refreshGui();
}


void RpmBuildPlugin::newReleaseInfoSelected()
{
    updateTab();

    refreshGui();
}


void RpmBuildPlugin::buildProcessStopped(ReleaseInfo *)
{
    refreshGui();
}


void RpmBuildPlugin::specButtonPressed()
{
    QFileInfo fileInfo(currentReleaseInfo->getSpecFile());
    if (fileInfo.exists() == false || fileInfo.size() == 0) //spec jeszcze nie istnieje? użyj tamplate
    {
        qDebug() << QString("preparing template for spec file (%1 -> %2)")
                    .arg(Functions::dataPath("spec_template"), fileInfo.absoluteFilePath() );

        //przygotuj spec pod bieżący projekt
        QFile src(Functions::dataPath("spec_template"));
        QFile dst(fileInfo.absoluteFilePath());
        src.open(QIODevice::ReadOnly);
        dst.open(QIODevice::WriteOnly);

        QString projName = currentReleaseInfo->getProjectInfo()->getName();

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

    //restore dialog's dimenstion
    QSettings settings;
    settings.beginGroup("Projects");
    settings.beginGroup(currentReleaseInfo->getProjectInfo()->getName());
    settings.beginGroup(currentReleaseInfo->getName());

    QByteArray dialogData = settings.value("Constants dialog").toByteArray();
    if (dialogData.size() > 0)
        dialog.restoreGeometry(dialogData);

    //fill it with constants
    for(const Pair &constant: getListOfConstants(currentReleaseInfo))
    {
        dialog.addConstant(constant.first, constant.second);

        //dialog.addSeparator();
    }

    //fill it with variables
    settings.beginGroup("spec variables");
    for (const QString &key: settings.childKeys())
    {
        const QString value = settings.value(key).toString();
        dialog.addVariable(key, value);
    }
    settings.endGroup();

    dialog.exec();

    if (dialog.result() == QDialog::Accepted)
    {
        //remove any existing variables
        settings.remove("spec variables");

        settings.beginGroup("spec variables");

        //save variables
        for(const std::pair<QString, QString> &variable: dialog.getVariables())
            settings.setValue(variable.first, QVariant(variable.second));

        settings.endGroup();
    }

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

    if (currentReleaseInfo)
    {
        BuildProcess *const buildProcess = findBuildProcess(currentReleaseInfo);
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
