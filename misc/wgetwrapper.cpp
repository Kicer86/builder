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

#include <QUrl>
#include <QProcess>
#include <QDebug>

#include "wgetwrapper.hpp"

WgetWrapper::WgetWrapper(const QUrl &url, const QString &output_file)
{
  process=new QProcess(this);

  connect(process, SIGNAL(readyRead()), this, SLOT(data()));
  connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finished(int,QProcess::ExitStatus)));

  process->setProcessChannelMode(QProcess::MergedChannels);
//   process->setWorkingDirectory(output_file);
  args << "--progress=dot" << url.toString() << "-O" << output_file;
}


WgetWrapper::~WgetWrapper()
{}


int WgetWrapper::get() const
{
  process->start("wget", args);
  //qDebug() << QString("starting: wget %1").arg(args.join(" "));
  return pid=process->pid();
}


void WgetWrapper::data()
{
  if (process->canReadLine())
  {
    QString line=process->readLine();
    line=line.left(line.length()-1);       //remove /n
    
    //analyze line
    QRegExp progressRegEx(" *([0-9])+[a-zA-Z][ \\.]+([0-9]+)%.*");
    if (progressRegEx.exactMatch(line))
    {
      int current=progressRegEx.capturedTexts()[2].toInt();
      emit dataReadProgress(current, 100);
    }
    qDebug() << line;
  }
}


void WgetWrapper::finished(int err, QProcess::ExitStatus)
{
  emit requestFinished(pid, err!=0);
}
