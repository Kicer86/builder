/*
    Singleton zawierający ogólne ustawienia programu
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

#include "stdafx.h"

#include "settings.hpp"

Settings::Settings()
{
    settings=new QSettings();
}


Settings::~Settings()
{
    delete settings;
}


Settings* Settings::instance()
{
    static Settings _instance;
    return &_instance;
}


QString Settings::getProjectsPath() const
{
    return settings->value("main/projects_path", "").toString();
}


QString Settings::getEnvPath() const
{
    return settings->value("main/env_path", "").toString();
}


Settings::EnvType Settings::getEnvType() const
{
    return static_cast<EnvType>(settings->value("main/env_type", Local).toInt());
}


QStringList Settings::getExtList() const
{
    QStringList ret;
    settings->beginGroup("main");
    int size=settings->beginReadArray("extensions");
    for (int i=0; i<size; i++)
    {
        settings->setArrayIndex(i);
        ret << settings->value("name").toString();
    }
    settings->endArray();
    settings->endGroup();

    return ret;
}


void Settings::setEnvPath(const QString &path)
{
    settings->setValue("main/env_path", path);
}


void Settings::setEnvType(Settings::EnvType type)
{
    settings->setValue("main/env_type", type);
}


void Settings::setProjsPath(const QString& path)
{
    settings->setValue("main/projects_path", path);
}


void Settings::setExtList(const QStringList& list)
{
    settings->beginGroup("main");
    settings->beginWriteArray("extensions");
    int i=0;
    foreach(QString ext, list)
    {
        settings->setArrayIndex(i++);
        settings->setValue("name", ext);
    }
    settings->endArray();
    settings->endGroup();
}
