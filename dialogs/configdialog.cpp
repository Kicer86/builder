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

#include "stdafx.h"

#include "configdialog.hpp"
#include "misc/settings.hpp"
#include "ui_configdialog.h"

ConfigDialog::ConfigDialog(QWidget* p, Qt::WindowFlags f): QDialog(p, f),
    ui(new Ui::ConfigDialog)
{
  ui->setupUi(this);

  connect(ui->locEnvButton, SIGNAL(pressed()), this, SLOT(locEnvButtonPressed()));
  connect(ui->extEnvButton, SIGNAL(pressed()), this, SLOT(extEnvButtonPressed()));
  connect(ui->projsBrowse, SIGNAL(pressed()), this, SLOT(projsBrowsePressed()));
  connect(ui->envBrowse, SIGNAL(pressed()), this, SLOT(envBrowsePressed()));

  connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));


  if (Settings::instance()->getEnvType()==Settings::Local)
  {
    ui->locEnvButton->setChecked(true);
    locEnvButtonPressed();
  }
  else
    ui->extEnvButton->setChecked(true);

  ui->envLine->setText(Settings::instance()->getEnvPath());
  ui->projsLine->setText(Settings::instance()->getProjectsPath());
  
  QStringList list=Settings::instance()->getExtList();
  QString edit;
  for(int i=0; i<list.size(); i++)
  {
    if (i+1<list.size())
      edit+= list[i] + "; ";
    else
      edit+= list[i]; 
  }
  ui->extLine->setText(edit);
}


ConfigDialog::~ConfigDialog()
{
  delete ui;
}


Settings::EnvType ConfigDialog::getEnvType() const
{
  if (ui->locEnvButton->isChecked())
    return Settings::Local;
  else
    return Settings::External;
}


QString ConfigDialog::getExtEnvPath() const
{
  return ui->envLine->text();
}


QString ConfigDialog::getProjsPath() const
{
  return ui->projsLine->text();
}


QStringList ConfigDialog::getExtList() const
{
  QStringList list=ui->extLine->text().split(";", QString::SkipEmptyParts);
  
  QStringList ret;
  foreach (QString str, list)
    ret << str.simplified();
  
  return ret;
}


void ConfigDialog::locEnvButtonPressed()
{
 ui->envLine->setDisabled(true);
 ui->envBrowse->setDisabled(true);
}


void ConfigDialog::extEnvButtonPressed()
{
  ui->envLine->setEnabled(true);
  ui->envBrowse->setEnabled(true);
}


void ConfigDialog::envBrowsePressed()
{
  QString path=
    QFileDialog::getExistingDirectory(this,
                                      tr("Browse for projects' dir"),
                                      ui->envLine->text());
  if (path!="")
    ui->envLine->setText(path);
}


void ConfigDialog::projsBrowsePressed()
{
  QString path=
    QFileDialog::getExistingDirectory(this,
                                      tr("Browse for projects' dir"),
                                      ui->projsLine->text());
  if (path!="")
    ui->projsLine->setText(path);
}
