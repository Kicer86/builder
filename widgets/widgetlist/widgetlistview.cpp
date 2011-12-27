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

#include <QContextMenuEvent>
#include <QDebug>
#include <QMenu>

#include "data_containers/projectsmanager.hpp"
#include "data_containers/releaseinfo.hpp"
#include "widgetlistitem.hpp"
#include "widgetdelegate.hpp"
#include "widgetlistproxymodel.hpp"
#include "widgetlistview.hpp"
#include "misc/functions.hpp"


WidgetListView::WidgetListView(QWidget* p): QListView(p)
{
    WidgetDelegate *delegate = new WidgetDelegate(this);
    setItemDelegate(delegate);

    widgets = new QHash<ReleaseInfo *, WidgetListItem *>;
    connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(itemClicked(const QModelIndex)));
}


WidgetListView::~WidgetListView()
{
    //foreach(WidgetListItem *widgetListItem, widgets)
    //    delete widgetListItem;

    delete widgets;
}


void WidgetListView::rowsAboutToBeRemoved(const QModelIndex& p, int start, int end)
{
    QListView::rowsAboutToBeRemoved(p, start, end);
}


void WidgetListView::rowsInserted(const QModelIndex& modelIndex, int start, int end)
{
    //stwórz dla nowych wierszy widgety które będą wyświetlane w liście
    for (int i = start; i <= end; i++)
    {
        QAbstractItemModel *m = model();
        const QModelIndex ch = m->index(i, 0);

        ReleaseInfo *releaseInfo = Functions::getReleaseInfo(ch);

        qDebug() << QString("inserting row in view for release %1").arg(releaseInfo->getName());

        if (widgets->contains(releaseInfo) == false)  // nie ma takiego elementu w bazie widgetów?
        {
            //create widget widget
            WidgetListItem *widgetListItem = new WidgetListItem(releaseInfo);
            connect(widgetListItem, SIGNAL(rerender(WidgetListItem*)), this, SLOT(itemReload(WidgetListItem*)));  //update index which needs it

            //zapisz widget w bazie
            widgets->insert(releaseInfo, widgetListItem);
        }
    }

    QAbstractItemView::rowsInserted(modelIndex, start, end);
    itemChanged();              //potraktuj to także jako edycję elementu -> odświeżymy widok
}


void WidgetListView::contextMenuEvent(QContextMenuEvent *e)
{
    //find item at position of click
    currentItem = indexAt(e->pos());

    QMenu menu(tr("Release actions"), this);
    menu.addAction(tr("Copy release"), this, SLOT(copyItem()));
    menu.exec(e->globalPos());
}


const QHash<ReleaseInfo *, WidgetListItem*> *WidgetListView::getWidgets() const
{
    return widgets;
}


void WidgetListView::dumpModel(const QModelIndex& index) const
{
    if (index.isValid() == false)
        return;

    qDebug() << QString("Item %1 r:%2 c:%3 data:%4").arg(index.data().toString()).arg(index.row()).arg(index.column()).arg(index.data(Qt::UserRole + 1).toInt());

    //zejdź w głąb
    dumpModel(index.child(0, 0));    //zejdź w dół - dziecko
    dumpModel(index.child(index.row()+1,index.column()));   //zejdź w dół - dziecko
    dumpModel(index.child(index.row(), index.column()+1));  //zejdź w dół - dziecko

    dumpModel(index.sibling(index.row()+1, index.column())); //zejdź w dół - wiersz niżej
    dumpModel(index.sibling(index.row(), index.column()+1)); //zejdź w dół - kolumna w prawo
}


WidgetListItem* WidgetListView::getProjectWidget(const QModelIndex& index) const
{
    //odnajdź ten element na liście
    ReleaseInfo *releaseInfo = Functions::getReleaseInfo(index);
    assert(widgets->contains(releaseInfo));

    return (*widgets)[releaseInfo];
}


void WidgetListView::itemClicked(const QModelIndex& index)
{
    currentItem = index;
    ReleaseInfo *rI = getProjectWidget(index)->getReleaseInfo();
    emit itemClicked(rI);
}


void WidgetListView::itemChanged()
{
    model()->sort(0, Qt::AscendingOrder);
}


void WidgetListView::itemReload(WidgetListItem *)
{
   // QModelIndex index = ProjectsManager::instance()->findIndex(item->getReleaseInfo());

   // dataChanged(index, index); //only this one works :/
}


void WidgetListView::copyItem()
{
    ReleaseInfo *releaseInfo = Functions::getReleaseInfo(currentItem);

    ProjectsManager::instance()->copyRelease(*releaseInfo);
}
