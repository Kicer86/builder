/*
    Klasa odpowiedzialna za wyświetlanie informacji o projekcie.
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

#include <QPlainTextEdit>
#include <QDebug>
#include <QListView>
#include <QStringListModel>
#include <QTemporaryFile>
#include <QProcess>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QApplication>

#include "projectinfowidget.hpp"
#include "data_containers/editorsmanager.hpp"
#include "data_containers/projectinfo.hpp"
#include "data_containers/releaseinfo.hpp"
#include "misc/settings.hpp"
#include "misc/sandboxprocess.hpp"
#include "misc/estimator.hpp"
#include "misc/functions.hpp"


static class HtmlDelegate:public QStyledItemDelegate
{
  public:
    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
      QApplication::style()->drawPrimitive( QStyle::PE_PanelItemViewItem, &option, painter );

      QLabel label(index.data().toString()); //create QLabel
      label.setAttribute(Qt::WA_OpaquePaintEvent, true);
      label.setAttribute(Qt::WA_NoSystemBackground, true);
      painter->save();
      painter->translate(option.rect.topLeft());

      //dostosuj szerokość widgetu do szerokości listy
      label.resize(option.rect.width(), label.sizeHint().height());

      //wyrysuj widget
      label.render(painter, QPoint() );

      painter->restore();
//       QStyledItemDelegate::paint(painter, option, index);
    }
} htmlDelegate;


ProjectInfoWidget::ProjectInfoWidget( QWidget* p, Qt::WindowFlags f):
    QWidget(p,f),releaseInfo(0)
{
  ui =new Ui_ProjectInfoForm();
  ui->setupUi(this);

  //disable buttons by default
  ui->editDowloadScriptButton->setDisabled(true);
  ui->specButton->setDisabled(true);
  ui->luaDebugButton->setDisabled(true);
  ui->showMacrosButton->setDisabled(true);

  //fill up local and remote boxes
  localInfoModel=new QStringListModel(this);
  currentInfoModel=new QStringListModel(this);

  localInfoView=new QListView;
  localInfoView->setModel(localInfoModel);
  currentInfoView=new QListView;
  currentInfoView->setModel(currentInfoModel);
  currentInfoView->setItemDelegate(&htmlDelegate);

  QVBoxLayout *localVersionLayout=new QVBoxLayout;
  QVBoxLayout *currentVersionLayout=new QVBoxLayout;

  localVersionLayout->addWidget(localInfoView);
  currentVersionLayout->addWidget(currentInfoView);

  ui->localVersionBox->setLayout(localVersionLayout);
  ui->remoteVersionBox->setLayout(currentVersionLayout);

  //progress bar
  ui->progressBar->setDisabled(true);

  connect(ui->editDowloadScriptButton, SIGNAL(pressed()), this, SLOT(editDowloadScriptButtonPressed()));
  connect(ui->specButton, SIGNAL(pressed()), this, SLOT(specButtonPressed()));
  connect(ui->showMacrosButton, SIGNAL(pressed()), this, SLOT(showMacrosButtonPressed()));
  connect(ui->updateButton, SIGNAL(pressed()), this, SLOT(updateButtonPressed()));
  connect(ui->downloadButton, SIGNAL(pressed()), this, SLOT(downloadButtonPressed()));
  connect(ui->buildButton, SIGNAL(pressed()), this, SLOT(buildButtonPressed()));
  connect(ui->fastBuildButton, SIGNAL(pressed()), this, SLOT(fastBuildButtonPressed()));
}


ProjectInfoWidget::~ProjectInfoWidget()
{
  delete ui;
}


void ProjectInfoWidget::setProjectRelease(ReleaseInfo* ri)
{
  if (releaseInfo)
    disconnect(releaseInfo);  //rozłącz połączenia poprzedniego projektu
  else  //pierwszy projekt
  {
    ui->editDowloadScriptButton->setEnabled(true);
    ui->specButton->setEnabled(true);
    ui->luaDebugButton->setEnabled(true);
    ui->showMacrosButton->setEnabled(true);

    ui->updateButton->setEnabled(true);
    ui->downloadButton->setEnabled(true);
    ui->buildButton->setEnabled(true);
    ui->fastBuildButton->setEnabled(true);
  }

  releaseInfo=ri;
  connect(releaseInfo, SIGNAL(changed(int)), this, SLOT(refresh(int)));

  ui->buildMessages->setDocument(releaseInfo->getBuildMesages());
  refresh(ReleaseInfo::AllChanged);
}


void ProjectInfoWidget::refresh(int type)
{
  if (releaseInfo)
  {
    if (type & ReleaseInfo::ProgressChange) //zmiana progressBara
    {
      ui->progressBar->setMaximum(releaseInfo->getProgressTotal());
      ui->progressBar->setValue(releaseInfo->getProgressDone());

      //dodatkowe iformacje progressBara
      ReleaseInfo::State state=releaseInfo->getState();
      int d=releaseInfo->getProgressDone();
      int t=releaseInfo->getProgressTotal();
      if (t>0)
      {
        if (state==ReleaseInfo::Downloading )
          ui->progressBar->setFormat(tr("%5: %p% %1/%2 (%3/%4)")
                                 .arg(sizeToString(d),
                                      sizeToString(t),
                                      releaseInfo->getEstimator()->elapsed().toString("H:mm:ss"),
                                      releaseInfo->getEstimator()->estimate().toString("H:mm:ss"),
                                      releaseInfo->getDownloadedPkg()
                                     )
                                );
        else if (state==ReleaseInfo::Building)
          ui->progressBar->setFormat(tr("%p% (%1/%2)")
                                 .arg(
                                   releaseInfo->getEstimator()->elapsed().toString("H:mm:ss"),
                                   releaseInfo->getEstimator()->estimate().toString("H:mm:ss")
                                 )
                                );
      }
    }

    if (type & ReleaseInfo::StateChange)
    {
      ReleaseInfo::State state=releaseInfo->getState();
      ui->updateButton->setEnabled(state==ReleaseInfo::Nothing);
      ui->progressBar->setEnabled(state!=ReleaseInfo::Nothing);

      if (state==ReleaseInfo::Building)
      {
        ui->buildButton->setText(tr("Stop"));
        ui->fastBuildButton->setText(tr("Stop"));
      }
      else
      {
        ui->buildButton->setText(tr("Build"));
        ui->fastBuildButton->setText(tr("Install check"));
      }

      ui->projectName->setText(QString("%1: %2").arg(releaseInfo->getProjectInfo()->getName()).arg(releaseInfo->getName()));

      const ReleaseInfo::VersionList localVersion=*releaseInfo->getLocalVersions();
      const ReleaseInfo::VersionList currentVersion=*releaseInfo->getCurrentVersions();

      QStringList list;

      localInfoModel->removeRows(0,localInfoModel->rowCount());

      foreach(ProjectVersion pV, localVersion)
      {
        list << QString("%1: %2").arg(pV.getName()).arg(pV.getVersion());
      }
      localInfoModel->setStringList(list);

      list.clear();
      currentInfoModel->removeRows(0, currentInfoModel->rowCount());
      foreach(ProjectVersion pV, currentVersion)
      {
        QString pkgName=pV.getName();
        QString element=QString("%1: %2").arg(pkgName).arg(pV.getVersion());
        if (localVersion.contains(pkgName))
        {
          if ( localVersion[pkgName].getVersion()==pV.getVersion() )
            element=setColour(element, Qt::darkGreen);
          else
            element=setColour(element, Qt::red);
        }

        list <<element;
      }
      currentInfoModel->setStringList(list);
      ui->downloadButton->setEnabled(state==ReleaseInfo::Nothing && releaseInfo->getCurrentVersions()->isEmpty()==false && *(releaseInfo->getCurrentVersions())!=*(releaseInfo->getLocalVersions()) );

      if (state==ReleaseInfo::Nothing)
        ui->progressBar->reset();
    }
  }
}


void ProjectInfoWidget::editDowloadScriptButtonPressed()
{
  EditorsManager::instance()->editFile(releaseInfo->getDownloadScriptFile());
}


void ProjectInfoWidget::specButtonPressed()
{
  QFileInfo fileInfo(releaseInfo->getSpecFile());
  if (fileInfo.exists()==false || fileInfo.size()==0) //spec jeszcze nie istnieje? użyj tamplate
  {
    qDebug() << QString("preparing template for spec file (%1 -> %2)")
    .arg(dataPath("spec_template"), fileInfo.absoluteFilePath() );

    //przygotuj spec pod bieżący projekt
    QFile src(dataPath("spec_template"));
    QFile dst(fileInfo.absoluteFilePath());
    src.open(QIODevice::ReadOnly);
    dst.open(QIODevice::WriteOnly);

    QString projName=releaseInfo->getProjectInfo()->getName();
    
    while (src.atEnd()==false)
    {
      QString line=src.readLine();

      QRegExp name("Name:.*$");
      if (name.exactMatch(line))       //uzupełnij nazwę projektu
        line=QString("Name:           %1\n").arg(projName);

      QRegExp mainSource("Source0:.*$");
      if (mainSource.exactMatch(line)) //uzupełnij główne źródło
        line=QString("Source0:        __FILEURL_%1__\n").arg(projName);

      QRegExp version("%define         version.*$");
      if (version.exactMatch(line))    //wersja programu
        line=QString("%define         version __VERSION_%1__\n").arg(projName);

      dst.write(line.toUtf8());
    }

    src.close();
    dst.close();
  }

  EditorsManager::instance()->editFile(fileInfo.absoluteFilePath());
}


void ProjectInfoWidget::showMacrosButtonPressed()
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


void ProjectInfoWidget::buildButtonPressed()
{
  releaseInfo->buildPkg(ReleaseInfo::Normal);
}


void ProjectInfoWidget::fastBuildButtonPressed()
{
  releaseInfo->buildPkg(ReleaseInfo::Fast);
}


void ProjectInfoWidget::downloadButtonPressed()
{
  releaseInfo->downloadPkg();
}


void ProjectInfoWidget::updateButtonPressed()
{
  releaseInfo->update();
}
