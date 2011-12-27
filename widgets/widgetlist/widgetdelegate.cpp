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


#include <assert.h>

#include <QPainter>
#include <QApplication>

#include "widgetdelegate.hpp"
#include "widgetlistitem.hpp"
#include "widgetlistview.hpp"

WidgetDelegate::WidgetDelegate(WidgetListView *p): QStyledItemDelegate(p), view(p)
{}


WidgetDelegate::~WidgetDelegate()
{}


WidgetListItem* WidgetDelegate::getProjectWidget(const QModelIndex &idx) const
{
    return view->getProjectWidget(idx);
}


void WidgetDelegate::paintItem(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    painter->translate(option.rect.topLeft());
    WidgetListItem *pW = getProjectWidget(index);

    //dostosuj szerokość widgetu do szerokości listy
    pW->resize(option.rect.width(), pW->sizeHint().height());

    //draw the widget
    pW->prePaintEvent(index);
    pW->render(painter, QPoint() );

    painter->restore();
}


void WidgetDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QApplication::style()->drawPrimitive( QStyle::PE_PanelItemViewItem, &option, painter );

    paintItem(painter, option, index);
}


QSize WidgetDelegate::sizeHint(const QStyleOptionViewItem &, const QModelIndex &index) const
{
    const WidgetListItem *pW = getProjectWidget(index);

    const QSize viewSize( pW->sizeHint() );
    const QSize editorSize = pW->getEditor() == nullptr? viewSize : pW->getEditor()->sizeHint();  //create also editor's size

    QSize result;

    result.setHeight( viewSize.height() > editorSize.height()? viewSize.height(): editorSize.height() );
    result.setWidth( viewSize.width() > editorSize.width()? viewSize.width(): editorSize.width() );

    return result;
}


QWidget* WidgetDelegate::createEditor(QWidget *p, const QStyleOptionViewItem & , const QModelIndex &index) const
{
    WidgetListItem *editor = new WidgetListItem(getProjectWidget(index)); //stwórz klona
    editor->setParent(p);

    QWidget *list = dynamic_cast<QWidget *>(this->parent());
    if (list != nullptr)
        list->update();

    return editor;
}

