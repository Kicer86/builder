/*
    Widget reprezentujący projekt
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

#include <QCheckBox>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QModelIndex>
#include <QLabel>

#include "data_containers/releaseinfo.hpp"
#include "data_containers/projectinfo.hpp"
#include "misc/settings.hpp"
#include "misc/functions.hpp"

#include "widgetlistitem.hpp"
#include "widgets/imagewidget.hpp"
#include "data_containers/imagesmanager.hpp"
#include "data_containers/projectsmanager.hpp"

WidgetListItem::WidgetListItem(ReleaseInfo* pI):
        QWidget(),
        editor(false),
        origins(0),
        releaseInfo(pI)
{
    construct();
    setAttribute(Qt::WA_NoSystemBackground, true);

    connect(pI, SIGNAL(optionsChanged()), this, SLOT(updateValues()));
}


//copying constructor -> editor widget is being created
WidgetListItem::WidgetListItem(WidgetListItem* w):
        QWidget(),
        editor(true),
        origins(w),
        releaseInfo(w->getReleaseInfo())
{
    construct();
    resize(w->size());
    setAutoFillBackground(true);  //tło musi być!
}


WidgetListItem::~WidgetListItem()
{
    assert(editor == (origins > 0)); //jesli origins to editor (i vice versa)
//   if (origins)
//     origins->updateValues();       //niech się widget wizualny zaktualizuje
}


void WidgetListItem::construct()
{
    //setup view
    widget = new QWidget();
    projectLayout = new QGridLayout(widget);

    //add Title
    title = new QLabel(releaseInfo->getProjectInfo()->getName());
    projectLayout->addWidget(title, 0, 0);

    QLabel *releaseName = new QLabel(releaseInfo->getName());

    bool dwl = releaseInfo->getDownloadFlag();
    bool bld = releaseInfo->getBuildFlag();

    projectLayout->addWidget(releaseName, 0, 1);

    if (editor)
    {
        QHBoxLayout *lineLayout = new QHBoxLayout();

        QCheckBox *downloadEditor = new QCheckBox(tr("download"));
        QCheckBox *buildEditor = new QCheckBox(tr("build"));

        downloadEditor->setCheckState(dwl ? Qt::Checked : Qt::Unchecked);
        buildEditor->setCheckState(bld ? Qt::Checked : Qt::Unchecked);

        connect(downloadEditor, SIGNAL(stateChanged(int)), releaseInfo, SLOT(setDownloadOption(int)));
        connect(buildEditor, SIGNAL(stateChanged(int)), releaseInfo, SLOT(setBuildOption(int)));

        lineLayout->addWidget(downloadEditor);
        lineLayout->addWidget(buildEditor);

        projectLayout->addLayout(lineLayout, 1, 0, 1, 2);
    }
    else  //view
    {
        QHBoxLayout *lineLayout = new QHBoxLayout();

        download = new QLabel();
        build = new QLabel();

        lineLayout->addWidget(download);
        lineLayout->addWidget(build);

        projectLayout->addLayout(lineLayout, 1, 0, 1, 2);

        //connect(releaseInfo, SIGNAL(changed()), this, SLOT(updateValues()));  //update itself when projectInfo signals change
    }

    pixmap = new ImageWidget(this);
    connect(pixmap, SIGNAL(rerender()), this, SLOT(internalRepaint()));

    //główny layout
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(pixmap);
    mainLayout->addWidget(widget, 1);

    if (editor == false)
        updateValues();   //zaktualizuj widok
}


void WidgetListItem::updateValues()
{
    assert(editor == false); //funkcja wywoływana tylko w trybie view

    //print release info
    bool dwl = releaseInfo->getDownloadFlag();
    bool bld = releaseInfo->getBuildFlag();

    download->setText(QString(tr("download: %1")
                              .arg(dwl ?
                                   setColour(tr("yes"), Qt::darkGreen) :
                                   setColour(tr("no"),  Qt::red)
                                  )
                             )
                     );

    build->setText(QString(tr("build: %1")
                           .arg(bld ?
                                setColour(tr("yes"), Qt::darkGreen) :
                                setColour(tr("no"),  Qt::red)
                               )
                          )
                  );

    pixmap->clear();

    const int icoSize = 32;

    //set icon according to release's state
    if (bld)
        pixmap->appendLayer( ImagesManager::instance()->getImage("build.png", icoSize) );
    else if (dwl)
        pixmap->appendLayer( ImagesManager::instance()->getImage("download.png", icoSize) );
    else
        pixmap->appendLayer( ImagesManager::instance()->getImage("off.png", icoSize) );

    //any action in progress??
    if ( releaseInfo->getState() != ReleaseInfo::State::Nothing )
        pixmap->appendLayer(ImagesManager::instance()->getImage("progress.svg", icoSize));
}


ReleaseInfo* WidgetListItem::getReleaseInfo() const
{
    return releaseInfo;
}


void WidgetListItem::internalRepaint()
{
    emit rerender(this);
}


void WidgetListItem::prePaintEvent(const QModelIndex &index)
{
    //Widget is going to be painted.
    //Before it update some values according to current data of model

    //the same project as in prevoius element?
    QModelIndex prevModel = index.sibling( index.row() - 1, index.column() );
    const bool prevIsValid = prevModel.isValid();

    const int prevModelData = prevIsValid? prevModel.data(Qt::UserRole + 1).toInt() : -1;
    const ProjectInfo *prevProjectInfo = prevIsValid? ProjectsManager::instance()->findRelease(prevModelData)->getProjectInfo() : 0;
    const ProjectInfo *curProjectInfo = releaseInfo->getProjectInfo();

    if (prevProjectInfo != curProjectInfo)      //prev and current have different ProjectInfo
        title->setEnabled(true);
    else
        title->setDisabled(true);
}
