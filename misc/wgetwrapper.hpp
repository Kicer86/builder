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


#ifndef WGETWRAPPER_HPP
#define WGETWRAPPER_HPP

#include <QObject>
#include <QStringList>
#include <QProcess>

class QUrl;

class WgetWrapper: public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(WgetWrapper);

    QProcess *process;
    QStringList args;
    mutable int pid;

  private slots:
    void data();
    void finished(int, QProcess::ExitStatus);

  public:
    WgetWrapper(const QUrl&, const QString&);
    virtual ~WgetWrapper();
    
    int get() const;

  signals:
    void dataReadProgress(int, int);
    void requestFinished(int, bool);
};

#endif // WGETWRAPPER_HPP
