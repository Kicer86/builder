/*
    WidgetDelegate: responsible for drawing WidgetListItem
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


#ifndef WIDGETDELEGATE_HPP
#define WIDGETDELEGATE_HPP

#include <QStyledItemDelegate>

class WidgetListView;
class WidgetListItem;

class WidgetDelegate: public QStyledItemDelegate
{
    WidgetListView *view;

    WidgetListItem *getProjectWidget(const QModelIndex &idx) const;
    void paintItem(QPainter* painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void paint(QPainter* painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

  public:
    explicit WidgetDelegate(WidgetListView *parent);
    virtual ~WidgetDelegate();

//     QRect childPos(const QModelIndex &parent, const QModelIndex &child);
};

#endif // WIDGETDELEGATE_HPP
