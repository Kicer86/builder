/*
    Klasa implementująca pobieranie paczek z internetu
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


#ifndef DOWNLOADER_HPP
#define DOWNLOADER_HPP

#include <QObject>
#include <QList>
#include <QPair>

#include "data_containers/projectversion.hpp"
#include "data_containers/releaseinfo.hpp"

class WgetWrapper;
class QHttp;
class QFtp;
class QFile;
class QEventLoop;
class QUrl;
class QUrlInfo;

class Downloader;

class DownloaderHelper: public QObject
{
    Q_OBJECT
    
  public:
    enum Mode
    {
      Check,
      Download,
    };

    enum ServerType
    {
      None,
      Index,
      SourceForge
    };
    
    struct DownloaderEntry
    {
      QString name;
      QString url;      
    };

    DownloaderHelper(const QUrl& url, Mode mode, ServerType type, const QString &localFile="", const Downloader *downloader=0);
    virtual ~DownloaderHelper();

    const QList<DownloaderEntry> *getEntries() const;
    int getState() const;
    
private:
    QList<DownloaderEntry> elementsList;
    QFtp *ftp;
    QHttp *http;
    WgetWrapper *wget;
    QFile *file;
    QEventLoop *localLoop;
    int awaitingId;

    int state;        //0 - ok, 1 - error
    Mode mode;
    ServerType type;

  private slots:
    void ftpDirectoryEntry(const QUrlInfo & i);
    void commandFinished( int id, bool error );
    void stateChanged(int state);
};

class Downloader : public QObject
{
    Q_OBJECT

  public:
    explicit Downloader(QObject* p = 0);
    virtual ~Downloader();

    ReleaseInfo::VersionList checkVersion(QByteArray script) const;
    bool download(const QUrl& url, const QString &localFile) const;

  signals:
    void progressUpdate(int, int);
    void progressUpdate(qint64, qint64);
};

#endif // DOWNLOADER_HPP
