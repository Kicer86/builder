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

#include <QFile>
#include <QProcess>

#include "editfile.hpp"

EditFile::EditFile(const QString &path, QObject* p):QObject(p)
{
  fileToEdit=new QFile(path, this);
  fileToEdit->open(QIODevice::ReadWrite);
  
  editor=new QProcess(this);
  //sygnał z opóźnieniem by nie zabijac zabijanego procesu
  connect(editor, SIGNAL(finished(int)), this, SLOT(fileClosed(int)), Qt::QueuedConnection);
  editor->start("kwrite", QStringList() << fileToEdit->fileName());
}


EditFile::~EditFile()
{
  if (editor->state()==QProcess::Running)
    editor->terminate(); //zakoncz proces edytora kulturalnie
}


void EditFile::fileClosed(int)
{
  emit done(this);
}
