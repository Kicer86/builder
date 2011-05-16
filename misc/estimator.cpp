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
#include <QDebug>

#include "data_containers/releaseinfo.hpp"
#include "estimator.hpp"

Estimator::Estimator(ReleaseInfo *ri):QObject(ri), releaseInfo(ri)
{
  connect(releaseInfo, SIGNAL(changed(int)), this, SLOT(update(int)));
  time=new QTime();
}


Estimator::~Estimator()
{
  delete time;
}


void Estimator::update(int change)
{
  if (change & ReleaseInfo::StateChange)
  {
    //rozpocznij pomiar
    time->start();
  }
}


QTime Estimator::estimate() const
{
  int timeLeft=0;
  if (releaseInfo->getProgressTotal() > 0)
  {
    //ile zajęło wykonanie bieżącego postępu?
    float delta=static_cast<float>(releaseInfo->getProgressDone())/releaseInfo->getProgressTotal();
    if (delta>0)
      timeLeft=time->elapsed()/delta;   //taki wzór mi wyszedł z proporcji ;)
  }
  
  return QTime(0,0).addMSecs(timeLeft);
}


QTime Estimator::elapsed() const
{
  return QTime(0,0).addMSecs(time->elapsed());
}
