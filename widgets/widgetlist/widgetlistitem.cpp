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
#include <QLabel>
#include <QGridLayout>
#include <QGroupBox>

#include "data_containers/releaseinfo.hpp"
#include "data_containers/projectinfo.hpp"
#include "misc/settings.hpp"
#include "misc/functions.hpp"

#include "widgetlistitem.hpp"
#include "widgets/imagewidget.hpp"
#include "data_containers/imagesmanager.hpp"
#include <QTimer>

WidgetListItem::WidgetListItem(const ProjectInfo* pI):
    QWidget(),
    editor(false),
    origins(0),
    projectInfo(pI)
{
  construct();
  setAttribute(Qt::WA_NoSystemBackground, true);
}


//konstruktor kopiujący (czyli tworzymy widget edytujący)
WidgetListItem::WidgetListItem(WidgetListItem* w):
    QWidget(),
    editor(true),
    origins(w),
    projectInfo(w->getProjectInfo())
{
  construct();
  resize(w->size());
  setAutoFillBackground(true);  //tło musi być!
  setFocusPolicy(Qt::StrongFocus);
}


void WidgetListItem::construct()
{
  groupBox=new QGroupBox(projectInfo->getName());
  projectLayout=new QGridLayout(groupBox);

  for (int i=0; i<projectInfo->getReleasesList()->size(); i++)
  {
    ReleaseInfo *release=projectInfo->getReleasesList()->at(i);
    QLabel *releaseName=new QLabel(release->getName());

    bool dwl=release->getDownloadFlag();
    bool bld=release->getBuildFlag();

    projectLayout->addWidget(releaseName, i, 0);

    if (editor)
    {
      QCheckBox *downloadEditor=new QCheckBox(tr("download"));
      QCheckBox *buildEditor=new QCheckBox(tr("build"));

      downloadEditor->setCheckState(dwl? Qt::Checked : Qt::Unchecked);
      buildEditor->setCheckState(bld? Qt::Checked : Qt::Unchecked);

      connect(downloadEditor, SIGNAL(stateChanged(int)), release, SLOT(downloadCheck(int)));
      connect(buildEditor, SIGNAL(stateChanged(int)), release, SLOT(buildCheck(int)));

      projectLayout->addWidget(downloadEditor, i, 1);
      projectLayout->addWidget(buildEditor, i, 2);
    }
    else  //view
    {
      download.append(new QLabel());        //nowo dodany QLabel będzie mial index i (jak odpowiadający mu release)
      build.append(new QLabel());           //jw

      projectLayout->addWidget(download[i], i, 1);
      projectLayout->addWidget(build[i], i, 2);
    }
  }

  pixmap=new ImageWidget(this);

  //główny layout
  QHBoxLayout *mainLayout=new QHBoxLayout(this);
  mainLayout->addWidget(pixmap);
  mainLayout->addWidget(groupBox,1);
  
  if (editor==false)
    updateValues();   //zaktualizuj widok
    
    QTimer *timer=new QTimer(this);
  connect (timer, SIGNAL(timeout()), this, SLOT(update()));
  timer->start(50);
}


void WidgetListItem::updateValues()
{
  assert(editor==false); //funkcja wywoływana tylko w trybie view

  for (int i=0; i<projectInfo->getReleasesList()->size(); i++)
  {
    ReleaseInfo *release=projectInfo->getReleasesList()->at(i);

    bool dwl=release->getDownloadFlag();
    bool bld=release->getBuildFlag();

    download[i]->setText(QString(tr("download: %1")
                                 .arg(dwl?
                                      setColour(tr("yes"), Qt::darkGreen):
                                      setColour(tr("no"),  Qt::red)
                                     )
                                )
                        );

    build[i]->setText(QString(tr("build: %1")
                              .arg(bld?
                                   setColour(tr("yes"), Qt::darkGreen):
                                   setColour(tr("no"),  Qt::red)
                                  )
                             )
                     );
  }

  projectInfo->updateStatus();

  pixmap->clear();
  if (projectInfo->getStatus()==ProjectInfo::Build || projectInfo->getStatus()==ProjectInfo::All)
  {
    pixmap->appendLayer(ImagesManager::instance()->getImage("build.png",48));
    pixmap->appendLayer(ImagesManager::instance()->getImage("progress.svg",48));
  }
  else if (projectInfo->getStatus()==ProjectInfo::Check)
    pixmap->appendLayer(ImagesManager::instance()->getImage("download.png",48));
  else
    pixmap->appendLayer(ImagesManager::instance()->getImage("off.png",48));
}


WidgetListItem::~WidgetListItem()
{
  assert(editor == (origins>0));   //jesli origins to editor (i vice versa)
  if (origins)
    origins->updateValues();       //niech się widget wizualny zaktualizuje
}


const ProjectInfo* WidgetListItem::getProjectInfo() const
{
  return projectInfo;
}


QRect WidgetListItem::childPos(int position)
{
  QRect ret;
  QWidget *w=projectLayout->itemAtPosition(position,0)->widget();  //znajdź widget w danym rzędzie 0. kolumnie (będzie to releaseName QLabel)
  ret.setTopLeft(w->pos());                                        //pozycja etykiety
  ret.setSize(QSize(width(), w->height()));                        //szerokość QGroupBoxa, wysokość etykiety

  return ret;
}
