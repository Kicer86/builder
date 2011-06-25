/*
    Lista widgetów
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

#include <QBoxLayout>
#include <QEvent>
#include <QDebug>
#include <QPainter>
#include <QStandardItemModel>
#include <QScrollBar>

#include "data_containers/projectinfo.hpp"
#include "data_containers/releaseinfo.hpp"
#include "data_containers/projectsmanager.hpp"
#include "widgetlistitem.hpp"
#include "widgetdelegate.hpp"
#include "widgetlistproxymodel.hpp"
#include "widgetlistview.hpp"


WidgetListView::WidgetListView(QWidget* p): QListView(p)
{
  WidgetDelegate *delegate=new WidgetDelegate(this);
  setItemDelegate(delegate);

  widgets=new QHash<int, WidgetListItem *>;
  connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(itemClicked(QModelIndex)));
}


WidgetListView::~WidgetListView()
{
  delete widgets;
}


void WidgetListView::rowsAboutToBeRemoved(const QModelIndex& p, int start, int end)
{
  QListView::rowsAboutToBeRemoved(p, start, end);
}


void WidgetListView::rowsInserted(const QModelIndex& modelIndex, int start, int end)
{
  //stwórz dla nowych wierszy widgety które będą wyświetlane w liście
  for(int i=start; i<=end; i++)
  {
    QAbstractItemModel *m=model();
    const QModelIndex ch=m->index(i,0);
    int id=ch.data(Qt::UserRole+1).toInt();  //rola: void ProjectsManager::registerProject(ProjectInfo* project)
    QString name=ch.data(Qt::DisplayRole).toString();
    qDebug() << QString("inserting row in view for project %1 with id %2").arg(name).arg(id);
    if (widgets->contains(id)==false)  // nie ma takiego elementu w bazie widgetów?
    {
      //znajdź projekt o zadanym id
      ProjectInfo *pi=ProjectsManager::instance()->findProject(id);

      //stwórz na jego podstawie widget
      WidgetListItem *piw=new WidgetListItem(pi, &modelIndex);
      piw->setAttribute(Qt::WA_PaintOnScreen);

      //zapisz widget w bazie
      widgets->insert(id, piw);
    }
  }
  QAbstractItemView::rowsInserted(modelIndex, start, end);
  itemEdited();              //potraktuj to także jako edycję elementu -> odświeżymy widok

//   qDebug() << "mark";
//   dumpModel(model()->index(0,0));
}


const QHash<int, WidgetListItem*> *WidgetListView::getWidgets() const
{
  return widgets;
}


void WidgetListView::dumpModel(const QModelIndex& index) const
{
  if (index.isValid()==false)
    return;

  qDebug() << QString("Item %1 r:%2 c:%3").arg(index.data().toString()).arg(index.row()).arg(index.column());

  //zejdź w głąb
//   dumpModel(index.child(0, 0));    //zejdź w dół - dziecko
//   dumpModel(index.child(index.row()+1,index.column()));   //zejdź w dół - dziecko
//   dumpModel(index.child(index.row(), index.column()+1));  //zejdź w dół - dziecko

//   dumpModel(index.sibling(index.row()+1, index.column())); //zejdź w dół - wiersz niżej
//   dumpModel(index.sibling(index.row(), index.column()+1)); //zejdź w dół - kolumna w prawo
}


WidgetListItem* WidgetListView::getProjectWidget(const QModelIndex& index) const
{
  //odnajdź ten element na liście
  int id=index.data(Qt::UserRole + 1).toInt();
  assert(widgets->contains(id));
  return (*widgets)[id];
}


void WidgetListView::itemClicked(const QModelIndex& index) const
{
  const ProjectInfo *pI=getProjectWidget(index)->getProjectInfo();

  //1. release
  if (pI->getReleasesList()->size()>0)
  {
    ReleaseInfo *rI=pI->getReleasesList()->at(0);
    ProjectsManager::instance()->showInfo(rI);
  }
}


void WidgetListView::itemEdited()
{
  model()->sort(0, Qt::AscendingOrder);
}


/*
void WidgetListView::drawBranches(QPainter* painter, const QRect& rect, const QModelIndex& index) const
{
//   QTreeView::drawBranches(painter, rect, index);
}


QRect WidgetListView::itemSize(const QModelIndex& index) const  //oblicz pozycję elementu (łącznie z dziećmi)
{
  int h=0;
  for (int i=0;i<index.row(); i++)
  {
    QModelIndex s=index.sibling(i, index.column());
    if (s.isValid())
      h+=itemDelegate()->sizeHint(QStyleOptionViewItem(), s).height();
  }
  QRect ret;
  ret.setSize(itemDelegate()->sizeHint(QStyleOptionViewItem(), index));  //wymiar
  ret.setTopLeft(QPoint(0,h));                                         //pozycja
  return ret;
}


int WidgetListView::horizontalOffset() const
{
  return horizontalScrollBar()->value ();
}


QModelIndex WidgetListView::indexAt(const QPoint& point) const
{
  //
  // very sleazy temporary implementation
  //
  for (int i = 0; i < model()->rowCount(rootIndex()); ++i)
  {
    QModelIndex index = model()->index (i, 0, rootIndex());

    if (visualRect(index).contains (point))
    {
      return index;
    }
  }

  return QModelIndex ();
}


bool WidgetListView::isIndexHidden(const QModelIndex& index) const
{
  return false;
}


QModelIndex WidgetListView::moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
  return QModelIndex ();
}


void WidgetListView::scrollTo(const QModelIndex& index, QAbstractItemView::ScrollHint hint)
{

}


void WidgetListView::setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags command)
{

}


int WidgetListView::verticalOffset() const
{
  return verticalScrollBar()->value ();
}


QRect WidgetListView::visualRect(const QModelIndex& index) const
{
  QRect ret;

  //sprawdź czy to parent czy dziecko (WidgetListView obsługuje tylko 2 poziomowe drzewa)
  QModelIndex parent=index.parent();
  qDebug() << index << parent << rootIndex();
  if (parent.isValid() && parent!=rootIndex()) //jesteśmy dzieckiem
  {
    //wyznacz pozycję wewnatrz rodzica (QGroupBox)
    WidgetDelegate *wD=dynamic_cast<WidgetDelegate *>(itemDelegate());
    assert(wD>0);
    ret=wD->childPos(parent, index);
  }
  else                  //jesteśmy rodzicem
    ret=itemSize(index);

  return ret;
}


QRegion WidgetListView::visualRegionForSelection(const QItemSelection& selection) const
{
  return QRegion();
}

*/
