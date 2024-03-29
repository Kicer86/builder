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

#include <assert.h>

#include <QString>
#include <QColor>
#include <QModelIndex>
#include <QStandardItem>

#include "builder-config.h"
#include "functions.hpp"
#include "data_containers/releaseinfo.hpp"
#include "data_containers/projectinfo.hpp"

#define DATA_POS (Qt::UserRole + 1)
#define TYPE_POS (Qt::UserRole + 2)

namespace Functions
{

    QString sizeToString(int value)
    {
        QString suffix="B";

        if (value>1024*1024)
        {
            value/=1024*1024;
            suffix="MiB";
        }
        else if (value>1024)
        {
            value/=1024;
            suffix="KiB";
        }

        return QString("%1%2").arg(value).arg(suffix);
    }


    QString setColour(const QString &str, const QColor &colour)
    {
        return QString("<span style=\"color: %1;\">%2</span>").arg(colour.name()).arg(str);
    }


    QString dataPath(const QString &path)
    {
        return QString("%1/%2").arg(BUILDER_DATADIR, path);
    }


    void* getDataInfo(const QModelIndex &index)
    {
        assert(index.isValid());

        void *result = index.data(DATA_POS).value<void *>();

        return result;
    }

    DataType getDataType(const QModelIndex &index)
    {
        assert(index.isValid());

        DataType type = static_cast<DataType>(index.data(TYPE_POS).value<int>() );   //blah :/

        return type;
    }


    ReleaseInfo* getReleaseInfo(const QModelIndex &index)
    {
        assert(index.isValid());

        ReleaseInfo *ret = nullptr;
        const DataType type = getDataType(index);

        switch (type)
        {
            case DataType::ReleaseInfo:
                ret = static_cast<ReleaseInfo*>(index.data(DATA_POS).value<void *>());
                break;

            case DataType::ProjectInfo:
                break;
        }

        return ret;
    }


    const ProjectInfo* getProjectInfo(const QModelIndex &index)
    {
        assert(index.isValid());

        const ProjectInfo *ret = nullptr;
        const DataType type = getDataType(index);

        switch (type)
        {
            case DataType::ReleaseInfo:
            {
                ReleaseInfo *releaseInfo = static_cast<ReleaseInfo*>(index.data(DATA_POS).value<void *>());
                ret = releaseInfo->getProjectInfo();
                break;
            }

            case DataType::ProjectInfo:
                ret = static_cast<ProjectInfo*>(index.data(DATA_POS).value<void *>());
                break;
        }

        return ret;
    }


    void setReleaseInfo(QStandardItem *item, ReleaseInfo *releaseInfo)
    {
        item->setData(qVariantFromValue(static_cast<void *>(releaseInfo)), DATA_POS);
        item->setData(qVariantFromValue(static_cast<int>(DataType::ReleaseInfo)), TYPE_POS);
    }


    void setReleaseInfo(QStandardItem *item, ProjectInfo *projectInfo)
    {
        item->setData(qVariantFromValue(static_cast<void *>(projectInfo)), DATA_POS);
        item->setData(qVariantFromValue(static_cast<int>(DataType::ProjectInfo)), TYPE_POS);
    }

} //namespace Functions
