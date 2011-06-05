/*
    Klasa zawierająca informacje nt projektu
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


#ifndef PROJECTINFO_HPP
#define PROJECTINFO_HPP

#include <QString>

#include "projectversion.hpp"

class ReleaseInfo;

class ProjectInfo
{
  public:
    ProjectInfo(QString n);
    virtual ~ProjectInfo();

    enum Status
    {
      Nothing,
      Check,
      Build,
      All
    };

    const QList<ReleaseInfo*> *getReleasesList() const;
    QString getName() const;
    int getId() const;
    Status updateStatus() const;
    Status getStatus() const;

  private:
    const int id;         //id projektu
    const QString name;
    QList <ReleaseInfo *> releasesList;
    mutable Status status;
};

#endif // PROJECTINFO_HPP
