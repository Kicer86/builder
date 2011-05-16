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


#ifndef EDITORSMANAGER_HPP
#define EDITORSMANAGER_HPP

#include <QObject>
#include <QList>

class QFile;
class EditFile;

class EditorsManager : public QObject
{
    Q_OBJECT

    QList<EditFile *> list;

    explicit EditorsManager(QObject* p = 0);

  private slots:
    void closeEditor(EditFile *);

  public:
    virtual ~EditorsManager();
    
    static EditorsManager *instance();
    void editFile(const QString& file);
};

#endif // EDITORSMANAGER_HPP
