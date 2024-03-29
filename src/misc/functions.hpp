/*
    Zbiór funkcji różnego przeznaczenia
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

class QColor;
class QString;
class QModelIndex;
class QStandardItem;

class ReleaseInfo;
class ProjectInfo;

namespace Functions
{

    enum class DataType
    {
        ProjectInfo,
        ReleaseInfo
    };

    QString sizeToString(int value);
    QString setColour(const QString &str, const QColor &colour);
    QString dataPath(const QString &path);

    void* getDataInfo(const QModelIndex &index);
    DataType getDataType(const QModelIndex &index);
    const ProjectInfo *getProjectInfo(const QModelIndex &index);
    ReleaseInfo* getReleaseInfo(const QModelIndex &index);

    void setReleaseInfo(QStandardItem *, ReleaseInfo *);
    void setReleaseInfo(QStandardItem *, ProjectInfo *);

}
