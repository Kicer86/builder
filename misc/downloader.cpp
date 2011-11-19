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
#include <QHttp>
#include <QDir>
#include <QEventLoop>

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <htmlparser.hpp>
#include <std_macros.hpp>
#include <str_op.hpp>
#include <std_macros.hpp>

#include "downloader.hpp"
#include "settings.hpp"
#include "wgetwrapper.hpp"


//set of all working DownloaderHelpers
typedef std::set<DownloaderHelper *> HelpersSet;
static HelpersSet helpers;

//lua "extensions"
static int searchForPkg(lua_State *state)
{
    //dostaniemy 2/3 parametry od lua: url, to czego szukać, i ew parametry dodatkowe :]
    int c = lua_gettop(state); //liczba parametrów
    if (c < 2 || c > 3)
    {
        qWarning() << QString("wrong number of parameters (%1) for 'searchForPkg' in LUA script").arg(c);
        lua_pushstring(state, "");
        return 0;
    }

    QByteArray rawUrl(lua_tostring(state, 1));        //gdzie szukać  (url do ftp lub http)
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

        if (url.host() == "code.google.com" || url.host() == "code.google.com")  //CG?
            type = DownloaderHelper::CodeGoogle;

        //... jakieś inne
    }

    DownloaderHelper downloadHelper;
    downloadHelper.fetch(url, DownloaderHelper::Check, type); //wylistuj dostępne wersje
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

    QByteArray ret;
    if (maxVersion.isEmpty() == false)
    {
        ret = maxVersion.text().toUtf8();
        qDebug() << "current version:" << ret;
    }
    else
    {
        qWarning() << "current version: could not find any matching file :(";
        ret = "";
    }

    lua_pushstring(state, ret.data());
    return 1;
}


DownloaderHelper::DownloaderHelper():
        ftp(0), http(0), wget(0), file(0), localLoop(0),
        state(0), mode(Mode::Check), type(DownloaderHelper::ServerType::None)
{
    //register download helper
    helpers.insert(this);
}


int DownloaderHelper::fetch(const QUrl& url, DownloaderHelper::Mode m, DownloaderHelper::ServerType t, const QString& localFile, const Downloader* downloader)
{
    mode = m;
    type = t;
    
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
                http = new QHttp(url.host(), url.port(80));
                connect(http, SIGNAL(requestFinished(int, bool)), this, SLOT(commandFinished(int, bool)));
                awaitingId = http->get(url.path());             //pobierz stronę z linkami do paczek
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
    else if (http)
        connect(http, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));

    localLoop = new QEventLoop;  //lokalna pętla potrzebna do wyczekiwania na połączenie
    return localLoop->exec();
}


void DownloaderHelper::killConnections()
{
    localLoop->exit(state = 1);   //quit with error (unfinished jobs)
}



void DownloaderHelper::stateChanged(int st)
{
    if ( (ftp && st == QFtp::Unconnected) ||
         (http && st == QHttp::Unconnected) 
       )
        qDebug() << "conenction closed";
}


void DownloaderHelper::commandFinished(int id, bool error)
{
    assert (ftp || http || wget);

    if (error)   //błąd?
    {
        state = 1; //ustaw status
        if (ftp)
            qWarning() << "ftp command finished:" << error << ftp->errorString();
        if (http)
            qWarning() << "http command finished:" << error << http->errorString();
        localLoop->exit(1);
    }
    else if (id == awaitingId)     //to na co czekaliśmy? (lista plików całkowicie pobrana lub zamknięcie połączenia)
    {
        switch (mode)
        {
            case Check:           //wyszukiwanie paczki
                if (ftp)
                {
                    //czekalismy na dane, przyszły, wiec koncz
                    //(nic nie trzeba robić)
                }
                else if (http)
                {
                    assert(type != None);
                    //parsuj html
                    HtmlParser parser(http->readAll().data());

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
                }
                break;

            case Download:                 //pobieranie paczki
                break;
        }

        //zamykamy połaczenie, zakoncz pętlę fazową ;)
        localLoop->exit(state);
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


int DownloaderHelper::getState() const
{
    return state;
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
    lua_State *luaState = lua_open();   // lua initiation
    int err = luaL_loadbuffer(luaState, script.data(), script.size(), "downloader");  //load script
    if (err == 0)
    {
        lua_register(luaState, "findPkg", searchForPkg);

        if (lua_pcall (luaState, 0, LUA_MULTRET, 0))
            qDebug() << lua_tostring(luaState, -1) << "\n";
    }

    ReleaseInfo::VersionList retList;
    const int retValues = lua_gettop(luaState);
    
    assert(retValues % 2 == 0);
    
    for (int i = 0; i < retValues; i += 2)
    {
        const QUrl url(lua_tostring(luaState, i + 2));
        ProjectVersion pV(url);
        QString name = lua_tostring(luaState, i + 1);
        pV.setName(name);
        retList[name] = pV;
    }
    return retList;
}


bool Downloader::download(const QUrl& url, const QString &localFile) const
{
    DownloaderHelper dH;
    dH.fetch(url, DownloaderHelper::Download, DownloaderHelper::None, localFile, this);
    return dH.getState() == 0;
}


void Downloader::killDownloadHelpers()
{
    //delete each helper
    FOREACH(helper, helpers)
        helper->killConnections();
}

