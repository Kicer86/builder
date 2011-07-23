/*
    <one line to give the program's name and a brief idea of what it does.>
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

WidgetDelegate::WidgetDelegate(WidgetListView* p): QStyledItemDelegate(p), view(p)
{}

WidgetDelegate::~WidgetDelegate()
{}

WidgetListItem* WidgetDelegate::getProjectWidget(const QModelIndex& idx) const
{
  return view->getProjectWidget(idx);
}


void WidgetDelegate::paintItem(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  painter->save();
  painter->translate(option.rect.topLeft());
  WidgetListItem *pW=getProjectWidget(index);

  //dostosuj szerokość widgetu do szerokości listy
  pW->resize(option.rect.width(), pW->sizeHint().height());

  //wyrysuj widget
  pW->render(painter, QPoint() );

  painter->restore();
}


void WidgetDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  /* option.state:
   65537 /1       rysowany
   73729 /8193    najechany
   106753 / 98561 kliknięty najechany/nienajechany

   10001/1
   1            QStyle::State_Active
       1        QStyle::State_Enabled

   12001/2001
    2           QStyle::State_MouseOver

   1A101/18101
    8           QStyle::State_Selected
    2
     1          QStyle::State_HasFocus
  */

    QApplication::style()->drawPrimitive( QStyle::PE_PanelItemViewItem, &option, painter );
//     QStyledItemDelegate::paint(painter, option, index);   //nie wyrysuje nic poza tłem, gdyż item nie zawiera zadnych danych
    paintItem(painter, option, index);
}


QSize WidgetDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  WidgetListItem *pW=getProjectWidget(index);
  return QSize( option.rect.width(), pW->sizeHint().height() );
}

QWidget* WidgetDelegate::createEditor(QWidget* p, const QStyleOptionViewItem& , const QModelIndex& index) const
{
  WidgetListItem *editor=new WidgetListItem(getProjectWidget(index)); //stwórz klona
  editor->setParent(p);
  return editor;
}

/*
QRect WidgetDelegate::childPos(const QModelIndex& parent, const QModelIndex& child)
{
  ProjectWidget *pW=getProjectWidget(parent);
  return pW->childPos(child.row()-parent.row()-1);  //pozycja childrena względem parenta
}*/
