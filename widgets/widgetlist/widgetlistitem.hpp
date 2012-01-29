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

#include <assert.h>

#include <QWidget>

class QGridLayout;
class QLabel;
class QModelIndex;

class ImageWidget;
class ProjectInfo;
class ReleaseInfo;


class WidgetListItem:public QWidget
{
    Q_OBJECT

    QWidget *widget;
    WidgetListItem *const origins;                   //wskaźnik na widget który jest edytowany
    const WidgetListItem *editor;                    //pointer to editor (if exists)
    QLabel *download, *build;
    QLabel *title;
    ImageWidget *pixmap;

    const ProjectInfo *projectInfo;                  //related projectInfo
    ReleaseInfo *releaseInfo;                        //related releaseInfo
    QGridLayout *projectLayout;

    void construct();
    void constructProject();
    void constructRelease();

  private slots:
    void updateValues();
    void internalRepaint();

  public:
    WidgetListItem(const ProjectInfo *, ReleaseInfo *);
    WidgetListItem(WidgetListItem *w);
    virtual ~WidgetListItem();

    ReleaseInfo *getReleaseInfo() const;

    const WidgetListItem* getEditor() const
    {
        //assert(editor != nullptr);
        return editor;
    }

    const WidgetListItem* getOrigins() const
    {
        assert(origins != nullptr);
        return origins;
    }

  signals:
    void rerender(WidgetListItem *);           //signal emited when widget needs to be repainted
};

#endif // PROJECTWIDGET_HPP
