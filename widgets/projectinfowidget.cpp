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

#include <assert.h>

#include <QStyledItemDelegate>
#include <QPainter>
#include <QStringListModel>
#include <QListView>
#include <QScrollBar>
#include <QDebug>

#include <std_macros.hpp>

#include "projectinfowidget.hpp"
#include "data_containers/editorsmanager.hpp"
#include "data_containers/projectinfo.hpp"
#include "data_containers/projectversion.hpp"
#include "data_containers/releaseinfo.hpp"
#include "misc/settings.hpp"
#include "misc/sandboxprocess.hpp"
#include "misc/estimator.hpp"
#include "misc/functions.hpp"


static class HtmlDelegate: public QStyledItemDelegate
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
        QWidget(p, f), releaseInfo(0)/*, autoScrool(true)*/
{
    //nothing usefull -> just protector (to be sure that only one ProjectInfoWidget has been declared)
    static bool declared;
    assert(declared == false);
    declared = true;

    ui = new Ui_ProjectInfoForm();
    ui->setupUi(this);

    //fill up local and remote boxes
    localInfoModel = new QStringListModel(this);
    remoteInfoModel = new QStringListModel(this);

    localInfoView = new QListView;
    localInfoView->setModel(localInfoModel);
    remoteInfoView = new QListView;
    remoteInfoView->setModel(remoteInfoModel);
    remoteInfoView->setItemDelegate(&htmlDelegate);

    QVBoxLayout *localVersionLayout = new QVBoxLayout;
    QVBoxLayout *currentVersionLayout = new QVBoxLayout;

    localVersionLayout->addWidget(localInfoView);
    currentVersionLayout->addWidget(remoteInfoView);

    ui->localVersionBox->setLayout(localVersionLayout);
    ui->remoteVersionBox->setLayout(currentVersionLayout);

    //progress bar
    ui->progressBar->setDisabled(true);

    connect(ui->editDowloadScriptButton, SIGNAL(clicked()), this, SLOT(editDowloadScriptButtonPressed()));
    connect(ui->updateButton, SIGNAL(clicked()), this, SLOT(updateButtonPressed()));
    connect(ui->downloadButton, SIGNAL(clicked()), this, SLOT(downloadButtonPressed()));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
//    connect(ui->buildMessages, SIGNAL(textChanged()), this, SLOT(logChanged()));
}


ProjectInfoWidget::~ProjectInfoWidget()
{
    delete ui;
}


void ProjectInfoWidget::addBuildPluginButtons(QLayout* buttons, const QString &name)
{
    QGroupBox *groupBox = new QGroupBox(tr("%1 build").arg(name), this);
    groupBox->setLayout(buttons);
    ui->buildPluginsLayout->addWidget(groupBox);;
}


void ProjectInfoWidget::addBuildPluginLogs(QWidget *tab,
                                           const QString &name,
                                           BuildPlugin* buildPlugin)
{
    ui->tabWidget->addTab(tab, name);
    refreshFunctions[tab] = buildPlugin;
}


void ProjectInfoWidget::setRelease(ReleaseInfo* ri)
{
    assert(ri);

    if (releaseInfo)
        disconnect(releaseInfo);  //rozłącz połączenia poprzedniego projektu
    else  //pierwszy projekt
    {
        ui->editDowloadScriptButton->setEnabled(true);
        ui->luaDebugButton->setEnabled(true);

        ui->updateButton->setEnabled(true);
        ui->downloadButton->setEnabled(true);
    }

    releaseInfo = ri;
    connect(releaseInfo, SIGNAL(statusChanged(int)), this, SLOT(refresh(int)));
    refresh(ReleaseInfo::AllChanged);
}


void ProjectInfoWidget::setReleaseInfo(ReleaseInfo* rI)
{
    setRelease(rI);

    //Update log in build tab
    //There is no need to update notactive tabs, becouse they will be updated when activated :)

    tabChanged(ui->tabWidget->currentIndex());
}


ReleaseInfo* ProjectInfoWidget::getCurrentRelease() const
{
    return releaseInfo;
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
            ReleaseInfo::State state = releaseInfo->getState();
//            const int d = releaseInfo->getProgressDone();
            const int t = releaseInfo->getProgressTotal();
            if (t > 0)
            {
                if (state == ReleaseInfo::State::Downloading )
                    ui->progressBar->setFormat(tr("%3: %p% (%1/%2)")
                                               .arg(/*sizeToString(d),
                                                    sizeToString(t),*/
                                                   releaseInfo->getEstimator()->elapsed().toString("H:mm:ss"),
                                                   releaseInfo->getEstimator()->estimate().toString("H:mm:ss"),
                                                   releaseInfo->getDownloadedPkg()
                                                   )
                                              );
                else if (state == ReleaseInfo::State::Building)
                    ui->progressBar->setFormat(tr("%p% (%1/%2)")
                                               .arg(
                                                   releaseInfo->getEstimator()->elapsed().toString("H:mm:ss"),
                                                   releaseInfo->getEstimator()->estimate().toString("H:mm:ss")
                                                   )
                                              );
                else
                    ui->progressBar->setFormat("%p%");
            }
        }

        if (type & ReleaseInfo::StateChange)
        {
            const ReleaseInfo::State state = releaseInfo->getState();

            if (state == ReleaseInfo::State::Nothing)
            {
                //ui->progressBar->reset();
                ui->progressBar->setMaximum(100);
                ui->progressBar->setValue(0);
            }

            ui->updateButton->setEnabled(state == ReleaseInfo::State::Nothing);
            ui->progressBar->setEnabled(state != ReleaseInfo::State::Nothing);
            ui->projectName->setText(QString("%1: %2").arg(releaseInfo->getProjectInfo()->getName()).arg(releaseInfo->getName()));

            const ReleaseInfo::VersionList localVersion = *releaseInfo->getLocalVersions();
            const ReleaseInfo::VersionList currentVersions = *releaseInfo->getCurrentVersions();

            localInfoModel->removeRows(0, localInfoModel->rowCount());

            QStringList list;
            foreach(ProjectVersion pV, localVersion)
                list << QString("%1: %2").arg(pV.getName()).arg(pV.getVersion());

            localInfoModel->setStringList(list);

            list.clear();
            remoteInfoModel->removeRows(0, remoteInfoModel->rowCount());

            foreach(ProjectVersion pV, currentVersions)
            {
                QString pkgName = pV.getName();
                QString element;

                if (pV.getStatus() != ProjectVersion::Status::Filled)   //not filled? error or empty
                {
                    element = QString("%1: %2").arg(pkgName).arg(pV.getErrorMsg());
                    element = Functions::setColour(element, Qt::red);
                }
                else if (localVersion.contains(pkgName))
                {
                    element = QString("%1: %2").arg(pkgName).arg(pV.getVersion());
                    if ( localVersion[pkgName].getVersion() == pV.getVersion() )
                        element = Functions::setColour(element, Qt::darkGreen);
                    else
                        element = Functions::setColour(element, Qt::darkYellow);
                }
                else
                {
                    element = QString("%1: %2").arg(pkgName).arg(pV.getVersion());
                    element = Functions::setColour(element, Qt::darkYellow);
                }

                list << element;
            }

            remoteInfoModel->setStringList(list);
            ui->downloadButton->setEnabled(state == ReleaseInfo::State::Nothing &&
                                           releaseInfo->getCurrentVersions()->isEmpty() == false &&
                                           *(releaseInfo->getCurrentVersions()) != *(releaseInfo->getLocalVersions()) );
        }
    }
}


//void ProjectInfoWidget::logWillChange()
//{
////    QScrollBar *bar = ui->buildMessages->verticalScrollBar();

////    autoScrool = bar->value() == bar->maximum();
//}


//void ProjectInfoWidget::logChanged()
//{
////    if (autoScrool)
////    {
////        QScrollBar *bar = ui->buildMessages->verticalScrollBar();
////        bar->setValue(bar->maximum());
////    }
//}


void ProjectInfoWidget::editDowloadScriptButtonPressed()
{
    EditorsManager::instance()->editFile(releaseInfo->getDownloadScriptFile());
}


void ProjectInfoWidget::downloadButtonPressed()
{
    releaseInfo->downloadPkg();
}


void ProjectInfoWidget::updateButtonPressed()
{
    releaseInfo->update();
}


void ProjectInfoWidget::tabChanged(int index)
{
    //find plugin of this tab
    QWidget *tab = ui->tabWidget->widget(index);  //get widget of current tab

    RefreshFunctions::const_iterator it = refreshFunctions.constFind(tab);   //find refresh function for this widget

    if (it != refreshFunctions.end())  //it may be not found -> for example when "main" or "non build" tab is chosen
    {
        BuildPlugin *bP = it.value();

        //call it
        bP->updateTab();
    }
}
