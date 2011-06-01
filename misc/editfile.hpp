/*
    Klasa uruchamiająca edytor dla wskazanego pliku.
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


#ifndef EDITFILE_HPP
#define EDITFILE_HPP

#include <QObject>

class QFile;
class QProcess;

class EditFile: public QObject
{
    Q_OBJECT

    QProcess *editor;
    QFile *fileToEdit;

    void constructor(QFile &);

  private slots:
    void fileClosed(int);

  public:
    explicit EditFile(const QString& path, QObject* p = 0);
    virtual ~EditFile();

  signals:
    void done(EditFile*);
};

#endif // EDITFILE_HPP
