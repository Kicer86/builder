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

#include <QRegExp>
#include <QStringList>
#include <QDebug>
#include <QSettings>

#include "projectversion.hpp"
#include <misc/settings.hpp>

ProjectVersion::ProjectVersion(const QString& filePart):
    status(Status::Empty), majorN(0), minorN(0),release(0), build(0)
{
    setValues(filePart);
}


ProjectVersion::ProjectVersion(const QUrl& u):
    status(Status::Empty), majorN(0), minorN(0),release(0), build(0)
{
    url=u;
    setValues(u.toString());
}


ProjectVersion::ProjectVersion():
    status(Status::Empty), majorN(0), minorN(0),release(0), build(0)
{}


void ProjectVersion::setValues(const QString& partialUrl)
{
    status = Status::Filled;

    rawVersion=partialUrl;             //partialUrl to częsciowy (wersja + rozszerzenie]) lub cały url do pliku
    QFileInfo fileInfo(partialUrl);
    //                major      minor         release     build
    QRegExp version(".*([0-9]+)[-.]([0-9]+)[-.]?([0-9]*)[-.]?([0-9]*).*");
    version.exactMatch(fileInfo.fileName());

    len=1;
    while (len<=version.captureCount() && version.capturedTexts()[len]!="")
        len++;

    len--;

    if (len>=1)
        majorN=(version.capturedTexts()[1]).toInt();
    if (len>=2)
        minorN=(version.capturedTexts()[2]).toInt();
    if (len>=3)
        release=(version.capturedTexts()[3]).toInt();
    if (len>=4)
        build=(version.capturedTexts()[4]).toInt();

    QRegExp extension(QString(".*\\.(%1)$")
                      .arg(Settings::instance()->getExtList().join("|"))
                      );

    if ( extension.exactMatch(fileInfo.fileName()) )
        ext=extension.capturedTexts()[1];
}


void ProjectVersion::setLocalFile(const QFileInfo& file)
{
    //usuń z nazwy scieżkę do projektów
    QString absoluteFilePath=file.absoluteFilePath();

    //usuń pierwsze znaki (zgodnie z długością scieżki do projektów)
    relativePath= absoluteFilePath.mid(Settings::instance()->getProjectsPath().length());
}


void ProjectVersion::setName(const QString& n)
{
    name=n;
}


QFileInfo ProjectVersion::getLocalFile() const
{
    QFileInfo ret(Settings::instance()->getProjectsPath()+relativePath);
    return ret;
}


//zwraca:
// 1 jeśli this jest > od pV,
// -1 jeśli this jest < od pV,
// 0 jeśli równe
int ProjectVersion::compare(const ProjectVersion& pV) const
{
    if (getStatus() != Status::Filled && pV.getStatus() == Status::Filled)
        return -1;
    if (getStatus() == Status::Filled && pV.getStatus() != Status::Filled)
        return 1;
    if (getStatus() != Status::Filled && pV.getStatus() != Status::Filled)
        return 0;

    if (majorN<pV.majorN)
        return -1;
    if (majorN>pV.majorN)
        return 1;

    if (minorN<pV.minorN)
        return -1;
    if (minorN>pV.minorN)
        return 1;

    if (release<pV.release)
        return -1;
    if (release>pV.release)
        return 1;

    if ( build<pV.build)
        return -1;
    if ( build>pV.build)
        return 1;

    return extCompare(pV);
}

//zwraca:
// 1 jeśli this ma "lepsze" rozszerzenie od od pV,
// -1 jeśli this ma "gorsze" rozszerzenie od od pV,
// 0 jeśli rozszerzenia takie same równe
int ProjectVersion::extCompare(const ProjectVersion& pV) const
{
    QStringList extList=Settings::instance()->getExtList() << "";

    int i1=extList.indexOf(ext);
    int i2=extList.indexOf(pV.ext);

    if (i1==-1)
    {
        qDebug() << "unknown extension:" << ext;
        i1=255;
    }

    if (i2==-1)
    {
        qDebug() << "unknown extension:" << pV.ext;
        i2=255;
    }

    return i1<i2? 1: (i1>i2? -1: 0);
}


QString ProjectVersion::text() const
{
    return rawVersion;
}


QString ProjectVersion::getVersion() const
{
    QString ret="-";
    if (len>=1)
        ret=QString("%1").arg(majorN);

    if (len>=2)
        ret+=QString(".%1").arg(minorN);

    if (len>=3)
        ret+=QString(".%1").arg(release);

    if (len>=4)
        ret+=QString(".%1").arg(build);
    return ret;
}


QString ProjectVersion::getExtension() const
{
    if (getStatus() != Status::Filled || len==0)
        return "-";
    else
        return ext;
}


const QString& ProjectVersion::getName() const
{
    return name;
}


const QUrl& ProjectVersion::getPkgUrl() const
{
    return url;
}


void ProjectVersion::setPkgUrl(const QUrl &u)
{
    url=u;
}


ProjectVersion::Status ProjectVersion::getStatus() const
{
    return status;
}


bool ProjectVersion::save(QSettings* settings) const
{
    if (status == Status::Filled)
    {
        settings->setValue("name", name);
        settings->setValue("major", majorN);
        settings->setValue("minor", minorN);
        settings->setValue("release", release);
        settings->setValue("build", build);
        settings->setValue("length", len);
        settings->setValue("phase", phase);
        settings->setValue("ext", ext);
        settings->setValue("url", url);
        settings->setValue("localPath", relativePath);
    }

    return status == Status::Filled;
}


void ProjectVersion::load(QSettings* settings)
{
    name=settings->value("name").toString();
    majorN=settings->value("major").toInt();
    minorN=settings->value("minor").toInt();
    release=settings->value("release").toInt();
    build=settings->value("build").toInt();
    len=settings->value("length").toInt();
    phase=settings->value("phase").toString();
    ext=settings->value("ext").toString();
    url=settings->value("url").toUrl();
    relativePath=settings->value("localPath").toString();

    status = name == ""? Status::Empty : Status::Filled;
}


bool ProjectVersion::operator<(const ProjectVersion& pV) const
{
    return compare(pV)==-1;
}


bool ProjectVersion::operator>(const ProjectVersion& pV) const
{
    return compare(pV)==1;
}


bool ProjectVersion::operator==(const ProjectVersion& pV) const
{
    return compare(pV)==0;
}


bool ProjectVersion::operator!=(const ProjectVersion& pV) const
{
    return compare(pV)!=0;
}

