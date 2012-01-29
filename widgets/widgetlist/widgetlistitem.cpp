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

WidgetListItem::WidgetListItem(const ProjectInfo *pI, ReleaseInfo* rI):
        QWidget(),
        origins(nullptr),
        editor(nullptr),            //edtor does not exist at this moment
        projectInfo(pI),
        releaseInfo(rI)
{
    construct();
    setAttribute(Qt::WA_NoSystemBackground, true);

    connect(releaseInfo, SIGNAL(optionsChanged()), this, SLOT(updateValues()));
}


//copying constructor -> editor widget is being created
WidgetListItem::WidgetListItem(WidgetListItem* w):
        QWidget(),
        origins(w),
        editor(nullptr),            //we are editor itself
        releaseInfo(w->getReleaseInfo())
{
    origins->editor = this;             //set as as editor of our origin
    construct();
    resize(w->size());
    setAutoFillBackground(true);  //tło musi być!
}


WidgetListItem::~WidgetListItem()
{
    assert(origins == nullptr || origins->editor != nullptr);      //origins has to have us
//   if (origins)
//     origins->updateValues();       //niech się widget wizualny zaktualizuje

    if (origins != nullptr)
        origins->editor = nullptr;
}


void WidgetListItem::construct()
{
    download = build = nullptr;

    //setup view
    widget = new QWidget();
    projectLayout = new QGridLayout(widget);

    //main layout
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    if (releaseInfo != nullptr)
        constructRelease(mainLayout);
    else
        constructProject();

    mainLayout->addWidget(widget, 1);
}


void WidgetListItem::constructProject()
{
    //add Title
    title = new QLabel(projectInfo->getName());
    projectLayout->addWidget(title, 0, 0);
}


void WidgetListItem::constructRelease(QHBoxLayout *mainLayout)
{
    QLabel *releaseName = new QLabel(releaseInfo->getName());

    bool dwl = releaseInfo->getDownloadFlag();
    bool bld = releaseInfo->getBuildFlag();

    projectLayout->addWidget(releaseName, 0, 0);

    QVBoxLayout *lineLayout = new QVBoxLayout();

    if (origins != nullptr)
    {
        QCheckBox *downloadEditor = new QCheckBox(tr("download"));
        QCheckBox *buildEditor = new QCheckBox(tr("build"));

        downloadEditor->setCheckState(dwl ? Qt::Checked : Qt::Unchecked);
        buildEditor->setCheckState(bld ? Qt::Checked : Qt::Unchecked);

        connect(downloadEditor, SIGNAL(stateChanged(int)), releaseInfo, SLOT(setDownloadOption(int)));
        connect(buildEditor, SIGNAL(stateChanged(int)), releaseInfo, SLOT(setBuildOption(int)));

        lineLayout->addWidget(downloadEditor);
        lineLayout->addWidget(buildEditor);

    }
    else  //view
    {
        download = new QLabel();
        build = new QLabel();

        lineLayout->addWidget(download);
        lineLayout->addWidget(build);

        //connect(releaseInfo, SIGNAL(changed()), this, SLOT(updateValues()));  //update itself when projectInfo signals change
    }

    projectLayout->addLayout(lineLayout, 0, 1, 2, 1, Qt::AlignJustify);

    pixmap = new ImageWidget(this);
    connect(pixmap, SIGNAL(rerender()), this, SLOT(internalRepaint()));

    //update main layout
    mainLayout->addWidget(pixmap);

    if (origins == nullptr)
        updateValues();   //zaktualizuj widok
}


void WidgetListItem::updateValues()
{
    assert(origins == nullptr); //funkcja wywoływana tylko w trybie view

    //print release info
    bool dwl = releaseInfo->getDownloadFlag();
    bool bld = releaseInfo->getBuildFlag();

    download->setText(QString(tr("download: %1")
                              .arg(dwl ?
                                   Functions::setColour(tr("yes"), Qt::darkGreen) :
                                   Functions::setColour(tr("no"),  Qt::red)
                                  )
                             )
                     );

    build->setText(QString(tr("build: %1")
                           .arg(bld ?
                                Functions::setColour(tr("yes"), Qt::darkGreen) :
                                Functions::setColour(tr("no"),  Qt::red)
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
