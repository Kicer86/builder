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

class WidgetListItem;

class WidgetListView: public QListView
{
    Q_OBJECT

    QHash<int, WidgetListItem *> *widgets;
    QWidget *backgroundWidget;

    QRect itemSize(const QModelIndex &index) const;
    void dumpModel(const QModelIndex &index) const;

  private slots:
    void itemClicked(const QModelIndex &index) const;
    void itemEdited();         //slot wywoływany przez widgetDelegate przez destrukcją edytora (celem odświeżenia modelu)

  protected:
//     virtual QRect visualRect(const QModelIndex& index) const;
//     virtual void scrollTo(const QModelIndex& index, ScrollHint hint = EnsureVisible);
//     virtual QModelIndex indexAt(const QPoint& point) const;
//     virtual QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers);
//     virtual int horizontalOffset() const;
//     virtual int verticalOffset() const;
//     virtual bool isIndexHidden(const QModelIndex& index) const;
//     virtual void setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags command);
//     virtual QRegion visualRegionForSelection(const QItemSelection& selection) const;

    virtual void rowsAboutToBeRemoved(const QModelIndex& parent, int start, int end);
    virtual void rowsInserted(const QModelIndex& parent, int start, int end);
//     virtual void drawBranches(QPainter* painter, const QRect& rect, const QModelIndex& index) const;

  public:
    explicit WidgetListView(QWidget* parent = 0);
    virtual ~WidgetListView();

    WidgetListItem* getProjectWidget(const QModelIndex& idx) const;
    const QHash< int, WidgetListItem* >* getWidgets() const;

};

#endif // WIDGETLIST_HPP
