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


#ifndef WIDGETLIST_HPP
#define WIDGETLIST_HPP

#include <QListView>
#include <QHash>

class ReleaseInfo;
class WidgetListItem;

namespace std
{
    template <class T> class shared_ptr;
}

class WidgetListView: public QListView
{
    Q_OBJECT

    typedef std::shared_ptr<WidgetListItem> WidgetListItemPtr;      //pointer to WidgetListItem
    typedef QHash<void *, WidgetListItemPtr> WidgetListView_List;

    WidgetListView_List widgets;             //lista wydgetów które są wyświetlane na liście
    QWidget *backgroundWidget;
    QModelIndex currentItem;                 //variable actualized in some rare ocasions like right click on item ;)

    QRect itemSize(const QModelIndex &index) const;
    void dumpModel(const QModelIndex &index) const;

  private slots:
    void itemClicked(const QModelIndex& index);
    void itemChanged();         //slot wywoływany przez widgetDelegate przez destrukcją edytora (celem odświeżenia modelu)
    void itemReload(WidgetListItem *item);

    //context menu:
    void copyItem();

  protected:
    virtual void rowsAboutToBeRemoved(const QModelIndex& parent, int start, int end);
    virtual void rowsInserted(const QModelIndex& parent, int start, int end);
    virtual void contextMenuEvent(QContextMenuEvent *);

  public:
    explicit WidgetListView(QWidget* parent = 0);
    virtual ~WidgetListView();

    WidgetListItem* getProjectWidget(const QModelIndex& idx) const;
    const WidgetListView_List *getWidgets() const;

  signals:
    void itemClicked(ReleaseInfo *) const;
};

#endif // WIDGETLIST_HPP
