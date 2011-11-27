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
class QNetworkAccessManager;
class QNetworkReply;
class QFtp;
class QFile;
class QEventLoop;
class QUrl;
class QUrlInfo;

class Downloader;

//helper for download - it does all the download operations.
//it's functions are called by lua scripts runned by Downloader class.
class DownloaderHelper: public QObject
{
        Q_OBJECT

    public:
        enum Mode
        {
            Check,
            Download
        };

        enum ServerType
        {
            None,
            Index,
            SourceForge,
            CodeGoogle
        };

        struct DownloaderEntry
        {
            QString name;
            QString url;
        };

        DownloaderHelper();
        virtual ~DownloaderHelper();

        int fetch(const QUrl&, Mode, ServerType, const QString &l = "", const Downloader * d = 0);  //return: 0 - ok, 1 - error
        void killConnections();

        const QList<DownloaderEntry> *getEntries() const;

    private:
        QList<DownloaderEntry> elementsList;
        QFtp *ftp;
        QNetworkAccessManager *http;
        WgetWrapper *wget;
        QFile *file;
        QEventLoop *localLoop;
        int awaitingId;                //for ftp connections
        QNetworkReply *awaitingReply;  //for http connections

        Mode mode;
        ServerType type;

    private slots:
        void ftpDirectoryEntry(const QUrlInfo & i);
        void commandFinished( int id, bool error );  //for ftp and wget
        void commandFinished( QNetworkReply * );     //for http
        void stateChanged(int state);
};


//class with download related functions interface
class Downloader : public QObject
{
        Q_OBJECT

    public:
        explicit Downloader(QObject* p = 0);
        virtual ~Downloader();

        ReleaseInfo::VersionList checkVersion(QByteArray script) const;
        bool download(const QUrl& url, const QString &localFile) const;

        static void killDownloadHelpers();

    signals:
        void progressUpdate(int, int);
        void progressUpdate(qint64, qint64);
};

#endif // DOWNLOADER_HPP
