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

#include <assert.h>

#include <set>

#include <QDebug>
#include <QFtp>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDir>
#include <QEventLoop>

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <OpenLibrary/htmlParser/htmlparser.hpp>
#include <OpenLibrary/common/std_macros.hpp>
#include <OpenLibrary/common/str_op.hpp>

#include "downloader.hpp"
#include "settings.hpp"
#include "wgetwrapper.hpp"


//set of all working DownloaderHelpers
typedef std::set<DownloaderHelper *> HelpersSet;
static HelpersSet helpers;

//lua "extensions"
static int searchForPkg(lua_State *state)  //search for package. "" is returned when something went wrong
{
    //dostaniemy 2/3 parametry od lua: url, to czego szukać, i ew parametry dodatkowe :]
    int c = lua_gettop(state); //liczba parametrów
    if (c < 2 || c > 3)
    {
        qWarning() << QString("wrong number of parameters (%1) for 'searchForPkg' in LUA script").arg(c);
        lua_pushstring(state, "");
        lua_pushinteger(state, 1);
        return 2;
    }

    const QByteArray rawUrl(lua_tostring(state, 1));  //gdzie szukać  (url do ftp lub http)
    QRegExp searchRegEx(lua_tostring(state, 2));      //czego szukać  (wyrażenie regularne)

    bool matchExt = false;                            //dopasowywać rozszerzenia? (czasem się przydaje - odfiltruje tylko te paczki które rozumiemy)
    if (c == 3)
        matchExt = lua_toboolean(state, 3);

    qDebug() << "fetching:" << rawUrl << "Looking for:" << searchRegEx.pattern();

    //rozpoznaj (w miarę możliwości) co to za typ serwera
    const QUrl url(rawUrl);
    DownloaderHelper::ServerType type = DownloaderHelper::Index;

    if (url.scheme() == "http")
    {
        if (url.host() == "sourceforge.net" || url.host() == "www.sourceforge.net")  //SF?
            type = DownloaderHelper::SourceForge;

        if (url.host() == "code.google.com" || url.host() == "www.code.google.com")  //CG?
            type = DownloaderHelper::CodeGoogle;

        //... jakieś inne
    }

    DownloaderHelper downloadHelper;
    const DownloaderHelper::FetchStatus status = downloadHelper.fetch(url, DownloaderHelper::Check, type); //wylistuj dostępne wersje

    //everything went ok?
    if (status == DownloaderHelper::FetchStatus::Ok)
    {
        ProjectVersion maxVersion;  //wersja zero

        foreach(DownloaderHelper::DownloaderEntry entry, *(downloadHelper.getEntries()))
        {
            QString toMatch;
            switch (type)
            {
                case DownloaderHelper::Index:       toMatch=entry.url;  break;
                case DownloaderHelper::SourceForge: toMatch=entry.name; break;
                case DownloaderHelper::CodeGoogle:  toMatch=entry.name; break;
                case DownloaderHelper::None: break;
            }

            if (searchRegEx.exactMatch(toMatch))  //przeszukaj wpisy pod kątem tych, pasujących do wzorca
            {
                ProjectVersion version;
                //sortowanie zalezy od typu serwera
                //w SF to nazwa linku się liczy, w przypadku indexów - link

                switch (type)
                {
                    case DownloaderHelper::Index:
                        version = ProjectVersion(entry.url);   //jeśli element spasował, to dodaj go do listy
                        break;

                    case DownloaderHelper::SourceForge:
                        version = ProjectVersion(entry.name);
                        version.setPkgUrl(QUrl(entry.url));    //zapisz element docelowy (w przypadku linków - href)
                        break;

                    case DownloaderHelper::CodeGoogle:
                        version = ProjectVersion(entry.name);  //name of package is enought
                        break;

                    case DownloaderHelper::None:
                        break;
                }

                //check if file extension matches
                if (matchExt == false || Settings::instance()->getExtList().contains(version.getExtension()))
                {
                    qDebug() << QString("matched package: %1").arg(version.text());

                    if (maxVersion < version)
                        maxVersion = version;
                }
                else
                    qDebug() << QString("%1 is matching regex in findPkg, but has unsufficient extension")
                                .arg(version.text());
            }
        }

        if (maxVersion.getStatus() == ProjectVersion::Status::Filled)
        {
            const QByteArray ret = maxVersion.text().toUtf8();
            qDebug() << "current version:" << ret;
            lua_pushstring(state, ret.data());
            lua_pushinteger(state, 0);   //ok
            return 2;
        }
        else
        {
            qWarning() << "current version: could not find any matching file :(";
            lua_pushstring(state, "none of files matched");
            lua_pushinteger(state, 1);
            return 2;
        }
    }
    else //problems with download
    {
        if (status == DownloaderHelper::FetchStatus::Error)
        {
            lua_pushstring(state, "connection problems");
            lua_pushinteger(state, 1);   //error
        }
        else
        {
            lua_pushstring(state, "connection killed");
            lua_pushinteger(state, 1);
        }

        return 2;
    }
}


DownloaderHelper::DownloaderHelper():
        ftp(0), http(0), wget(0), file(0), localLoop(0),
        mode(Mode::Check), type(DownloaderHelper::ServerType::None)
{
    //register download helper
    helpers.insert(this);
}


DownloaderHelper::FetchStatus DownloaderHelper::fetch(const QUrl& url, DownloaderHelper::Mode m, DownloaderHelper::ServerType t, const QString& localFile, const Downloader* downloader)
{
    mode = m;
    type = t;

    fetchStatus = FetchStatus::Ok;
    errorMessage = "Ok";

    switch (mode)
    {
        case Check:
            if (url.scheme() == "ftp")
            {
                ftp = new QFtp;

                connect(ftp, SIGNAL(commandFinished(int, bool)), this, SLOT(commandFinished(int, bool)));
                connect(ftp, SIGNAL(listInfo (const QUrlInfo &)), this, SLOT(ftpDirectoryEntry(const QUrlInfo &)));

                ftp->connectToHost(url.host(), url.port(21));   //nawiąż połączenie
                ftp->login();
                awaitingId = ftp->list(url.path());             //wylistuj katalogi
            }
            else if (url.scheme() == "http")
            {
                http = new QNetworkAccessManager();
                connect(http, SIGNAL(finished(QNetworkReply *)), this, SLOT(commandFinished(QNetworkReply *)));
                awaitingReply = http->get(QNetworkRequest(url));   //pobierz stronę z linkami do paczek
            }
            break;

        case Download:
            assert(downloader != 0);
            qDebug() << QString ("Downloading: %1\n\t\tto: %2").arg(url.toString()).arg(localFile);

            //check if local directory exists
            QDir directory(QFileInfo(localFile).absoluteDir());
            if (directory.exists() == false) //no?
                directory.mkpath(".");        //create it then

            wget = new WgetWrapper(url, localFile);
            connect(wget, SIGNAL(requestFinished(int, bool)), this, SLOT(commandFinished(int, bool)));
            connect(wget, SIGNAL(dataReadProgress(int, int)), downloader, SIGNAL(progressUpdate(int, int)));
            awaitingId = wget->get();
            break;
    }

    if (ftp)
        connect(ftp, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));

    localLoop = new QEventLoop;  //lokalna pętla potrzebna do wyczekiwania na połączenie
    localLoop->exec();
    return fetchStatus;
}


void DownloaderHelper::killConnections()
{
    fetchStatus = FetchStatus::Killed;   //quit with error (unfinished jobs)
    errorMessage = "connection killed";
    localLoop->exit(0);
}



void DownloaderHelper::stateChanged(int st)
{
    if (ftp && st == QFtp::Unconnected)
        qDebug() << "ftp connection closed";
}


void DownloaderHelper::commandFinished(int id, bool error)
{
    assert ( (ftp || wget) && http == 0);   //here we get only if ftp/wget is used

    if (error)   //błąd?
    {
        if (ftp)
        {
            qWarning() << "ftp command finished:" << error << ftp->errorString();
            errorMessage = ftp->errorString();
        }
        else if (wget)
            qWarning() << "wget command finished with error";

        fetchStatus = FetchStatus::Error;
        localLoop->exit(0);
    }
    else if (id == awaitingId)     //to na co czekaliśmy? (lista plików całkowicie pobrana lub zamknięcie połączenia)
    {
        //everything downloaded, quit
        fetchStatus = FetchStatus::Ok;
        localLoop->exit(0);
    }
}


void DownloaderHelper::commandFinished(QNetworkReply *reply)
{
    assert ( ftp ==0 && wget == 0 && http != 0);   //here we get only when http is used

    if (reply->error() != QNetworkReply::NoError )
    {
        reply->deleteLater();
        qWarning() << "http command finished with error:" << reply->errorString();
        errorMessage = reply->errorString();
        fetchStatus = FetchStatus::Error;
        localLoop->exit(0);
    }
    else if ( reply == awaitingReply ) //this is what we are waiting for?
    {
        assert(type != None);
        //parsuj html
        HtmlParser parser(reply->readAll().data());

        //wyłuskaj wszystkie linki
        std::vector<HtmlTag*> links;
        switch (type)
        {
            case Index:
                links = parser.findAll("a[href]");
                break;

            case SourceForge:
                links = parser.findAll("a[class=name][href]");
                break;

            case CodeGoogle:
                links = parser.findAll("td[class='vt id col_0'] a");

            case None:
                break;
        }

        for (uint i = 0; i < links.size(); i++)
        {
            DownloaderEntry entry;
            entry.name = Strings::stripBlanks(links[i]->getText()).c_str();
            entry.url = Strings::stripBlanks(links[i]->getAttr("href").value).c_str();

            elementsList << entry;
            qDebug() << QString("found package: %1 (%2)").arg(entry.name, entry.url);
        }

        //zamykamy połaczenie, zakoncz pętlę fazową ;)
        fetchStatus = FetchStatus::Ok;
        localLoop->exit(0);

        reply->deleteLater();
    }
}


DownloaderHelper::~DownloaderHelper()
{
    //remove helper from list of helpers
    HelpersSet::iterator helper = helpers.find(this);
    assert(helper != helpers.end());

    helpers.erase(helper);

    qDebug() << "killing downloader, closing connections";

    if (ftp)
    {
        disconnect(ftp);
        delete ftp;
    }

    if (http)
    {
        disconnect(http);
        delete http;
    }

    if (wget)
    {
        disconnect(wget);
        delete wget;
    }

    if (file)
        delete file;

    if (localLoop)
        delete localLoop;
}


const QList<DownloaderHelper::DownloaderEntry>* DownloaderHelper::getEntries() const
{
    return &elementsList;
}


void DownloaderHelper::ftpDirectoryEntry(const QUrlInfo& i)
{
    DownloaderEntry entry;
    entry.name = i.name();
    entry.url = i.name();
    elementsList << entry;
}


Downloader::Downloader(QObject* p): QObject(p)
{}


Downloader::~Downloader()
{}


ReleaseInfo::VersionList Downloader::checkVersion(QByteArray script) const
{
    ReleaseInfo::VersionList retList;
    lua_State *luaState = lua_open();   // lua initiation
    int err = luaL_loadbuffer(luaState, script.data(), script.size(), "downloader");  //load script
    if (err == 0)
    {
        lua_register(luaState, "findPkg", searchForPkg);

        if (lua_pcall(luaState, 0, LUA_MULTRET, 0) == 0)  //it's ok?
        {
            const int retValues = lua_gettop(luaState);

            assert(retValues % 3 == 0);

            for (int i = 0; i < retValues; i += 3)
            {
                const int status =  lua_tointeger(luaState, i + 3);   //status (0 - ok, 1 - error)
                const char * rawUrl = lua_tostring(luaState, i + 2);  //url or error (if status == 1)
                const QString name = lua_tostring(luaState, i + 1);
                if ( status == 0 ) //everything fine?
                {
                    const QUrl url(rawUrl);
                    ProjectVersion pV(url);
                    pV.setName(name);
                    retList[name] = pV;
                }
                else
                {
                    ProjectVersion pV;
                    pV.setError(rawUrl);
                    retList[name] = pV;  //empty version
                }
            }
        }
        else
            qDebug() << "lua error: " << lua_tostring(luaState, -1) << "\n";
    }

    lua_close(luaState);

    return retList;
}


bool Downloader::download(const QUrl& url, const QString &localFile) const
{
    DownloaderHelper dH;
    const DownloaderHelper::FetchStatus status = dH.fetch(url, DownloaderHelper::Download, DownloaderHelper::None, localFile, this);
    return status == DownloaderHelper::FetchStatus::Ok;
}


void Downloader::killDownloadHelpers()
{
    //delete each helper
    FOREACH(helper, helpers)
        helper->killConnections();
}

