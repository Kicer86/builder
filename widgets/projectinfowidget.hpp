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


#ifndef PROJECTINFOWIDGET_HPP
#define PROJECTINFOWIDGET_HPP

#include <QWidget>

#include "ui_projectinfowidget.h"

class QListView;
class QStringListModel;
class QPlainTextEdit;

class ReleaseInfo;

class ProjectInfoWidget:public QWidget
{
    Q_OBJECT

    Ui_ProjectInfoForm *ui;
    ReleaseInfo *releaseInfo;

    //info o projekcie
    QListView *localInfoView;
    QListView *remoteInfoView;
    QStringListModel *localInfoModel;
    QStringListModel *remoteInfoModel;

  private slots:
    void refresh(int type);
    void editDowloadScriptButtonPressed();
    void specButtonPressed();
    void showMacrosButtonPressed();
    void updateButtonPressed();
    void downloadButtonPressed();
    void buildButtonPressed();
    void fastBuildButtonPressed();

//   void refreshProgress();

  public:
    explicit ProjectInfoWidget(QWidget* p = 0, Qt::WindowFlags f = 0);
    virtual ~ProjectInfoWidget();

  public slots:
    void setProjectRelease(ReleaseInfo* pw);
};

#endif // PROJECTINFOWIDGET_HPP
