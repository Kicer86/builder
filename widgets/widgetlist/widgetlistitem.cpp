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
#include <QHBoxLayout>
#include <QLabel>

#include "data_containers/releaseinfo.hpp"
#include "data_containers/projectinfo.hpp"
#include "misc/settings.hpp"
#include "misc/functions.hpp"

#include "widgetlistitem.hpp"
#include "widgets/imagewidget.hpp"
#include "data_containers/imagesmanager.hpp"
#include "data_containers/projectsmanager.hpp"

WidgetListItem::WidgetListItem(ReleaseInfo* pI, const QModelIndex &mI):
    QWidget(),
    modelIndex(mI),
    editor(false),
    origins(0),
    releaseInfo(pI)
{
  construct();
  setAttribute(Qt::WA_NoSystemBackground, true);
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
//   setFocusPolicy(Qt::StrongFocus);
}


WidgetListItem::~WidgetListItem()
{
  assert(editor == (origins > 0)); //jesli origins to editor (i vice versa)
//   if (origins)
//     origins->updateValues();       //niech się widget wizualny zaktualizuje
}


void WidgetListItem::construct()
{
  widget = new QWidget();
  projectLayout = new QVBoxLayout(widget);

  //the same project as in prevoius element?
  QModelIndex prevModel = modelIndex.sibling(modelIndex.column(), modelIndex.row() - 1);

  if (prevModel.isValid() == false ||        //if prev is invalid or
      ProjectsManager::instance()->findRelease(prevModel.data(Qt::UserRole + 1).toInt())->getProjectInfo() !=
      releaseInfo->getProjectInfo()          //prev and current have different ProjectInfo
     )
  {
    //add Title
    projectLayout->addWidget(new QLabel(releaseInfo->getProjectInfo()->getName()));
  }

  QLabel *releaseName = new QLabel(releaseInfo->getName());

  bool dwl = releaseInfo->getDownloadFlag();
  bool bld = releaseInfo->getBuildFlag();

  projectLayout->addWidget(releaseName);

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

    projectLayout->addLayout(lineLayout);
  }
  else  //view
  {
    QHBoxLayout *lineLayout = new QHBoxLayout();

    download = new QLabel();
    build = new QLabel();

    lineLayout->addWidget(download);
    lineLayout->addWidget(build);

    projectLayout->addLayout(lineLayout);

    //connect(releaseInfo, SIGNAL(changed()), this, SLOT(updateValues()));  //update itself when projectInfo signals change
  }

  pixmap = new ImageWidget(this);
  connect(pixmap, SIGNAL(rerender()), this, SLOT(internalRepaint()));

//główny layout
  QHBoxLayout *mainLayout = new QHBoxLayout(this);
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

  //set icon according to release's state
  if (bld)
    pixmap->appendLayer( ImagesManager::instance()->getImage("build.png", 48) );
  else if (dwl)
    pixmap->appendLayer( ImagesManager::instance()->getImage("download.png", 48) );
  else
    pixmap->appendLayer( ImagesManager::instance()->getImage("off.png", 48) );

  //any action in progress??
  if ( releaseInfo->getState() != ReleaseInfo::Nothing )
    pixmap->appendLayer(ImagesManager::instance()->getImage("progress.svg", 48));
}


ReleaseInfo* WidgetListItem::getReleaseInfo() const
{
  return releaseInfo;
}


QRect WidgetListItem::childPos(int position)
{
  QRect ret;
  QWidget *w ;//= projectLayout->ititemAtPosition(position, 0)->widget();  //znajdź widget w danym rzędzie 0. kolumnie (będzie to releaseName QLabel)
  ret.setTopLeft(w->pos());                                        //pozycja etykiety
  ret.setSize(QSize(width(), w->height()));                        //szerokość QGroupBoxa, wysokość etykiety

  return ret;
}


void WidgetListItem::internalRepaint()
{
  emit rerender(modelIndex);
}
