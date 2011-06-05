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

class QLabel;
class ProjectInfo;
class QGridLayout;
class QGroupBox;

class WidgetListItem:public QWidget
{
    QGroupBox *groupBox;
    const bool editor; 
    WidgetListItem *origins;                   //wskaźnik na widget który jest edytowany
    QList<QLabel *> download, build;
    QLabel *pixmap;
    
    const ProjectInfo *projectInfo;
    QGridLayout *projectLayout;
    
    void construct();
    void updateValues();
        
  public:
    WidgetListItem(const ProjectInfo* pI);
    WidgetListItem(WidgetListItem* w);
    virtual ~WidgetListItem();
    
    const ProjectInfo *getProjectInfo() const;
    QRect childPos(int);              //odnajduje pozycję wydania wewnątrz QGroupBoxu
};

#endif // PROJECTWIDGET_HPP
