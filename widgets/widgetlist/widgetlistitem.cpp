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

#include "data_containers/releaseinfo.hpp"
#include "data_containers/projectinfo.hpp"
#include "misc/settings.hpp"
#include "misc/functions.hpp"

#include "widgetlistitem.hpp"

WidgetListItem::WidgetListItem(const ProjectInfo* pI)
    :QGroupBox(pI->getName()), editor(false), origins(0), projectInfo(pI)
{
  construct();
  setAttribute(Qt::WA_NoSystemBackground, true);
}


//konstruktor kopiujący (czyli tworzymy widget edytujący)
WidgetListItem::WidgetListItem(WidgetListItem* w)
    :QGroupBox(w->getProjectInfo()->getName()), editor(true), origins(w),
    projectInfo(w->getProjectInfo())
{
  construct();
  resize(w->size());
  setAutoFillBackground(true);  //tło musi być!
  setFocusPolicy(Qt::StrongFocus);
}


void WidgetListItem::construct()
{
  projectLayout=new QGridLayout(this);
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
      download=new QLabel();
      build=new QLabel();
      updateValues();

      projectLayout->addWidget(download, i, 1);
      projectLayout->addWidget(build, i, 2);
      
    }
  }
}


void WidgetListItem::updateValues()
{
  assert(editor==false); //funkcja wywoływana tylko w trybie view

  for (int i=0; i<projectInfo->getReleasesList()->size(); i++)
  {
    ReleaseInfo *release=projectInfo->getReleasesList()->at(i);

    bool dwl=release->getDownloadFlag();
    bool bld=release->getBuildFlag();

    download->setText(QString(tr("download: %1")
                              .arg(dwl?
                                   setColour(tr("yes"), Qt::darkGreen):
                                   setColour(tr("no"),  Qt::red)
                                  )
                             )
                     );

    build->setText(QString(tr("build: %1")
                           .arg(bld?
                                setColour(tr("yes"), Qt::darkGreen):
                                setColour(tr("no"),  Qt::red)
                               )
                          )
                  );
  }
}


WidgetListItem::~WidgetListItem()
{
  assert(editor == (origins>0));   //jesli origins to editor (i vice versa)
  if (origins)
    origins->updateValues();   //niech się widget wizualny zaktualizuje
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
