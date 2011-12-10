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


#ifndef PROJECTWIDGET_HPP
#define PROJECTWIDGET_HPP

#include <QWidget>

class QGridLayout;
class QLabel;
class QModelIndex;

class ImageWidget;
class ReleaseInfo;


class WidgetListItem:public QWidget
{
    Q_OBJECT

    QWidget *widget;
    const bool editor;                         //widget moze trwać w jednym z dwóch stanów: edytor i nieedytor ;)
    WidgetListItem *origins;                   //wskaźnik na widget który jest edytowany
    QLabel *download, *build;
    QLabel *title;
    ImageWidget *pixmap;

    ReleaseInfo *releaseInfo;                  //related releaseInfo
    QGridLayout *projectLayout;

    void construct();

  private slots:
    void updateValues();
    void internalRepaint();

  public:
    WidgetListItem(ReleaseInfo* pI);
    WidgetListItem(WidgetListItem* w);
    virtual ~WidgetListItem();

    ReleaseInfo *getReleaseInfo() const;

    QRect childPos(int);                          //odnajduje pozycję wydania wewnątrz QGroupBoxu
    void prePaintEvent(const QModelIndex &);

  signals:
    void rerender(WidgetListItem *);           //signal emited when widget needs to be repainted
};

#endif // PROJECTWIDGET_HPP
