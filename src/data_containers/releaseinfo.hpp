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


#ifndef RELEASEINFO_HPP
#define RELEASEINFO_HPP

#include <QProcess>

#include "data_containers/projectversion.hpp"

class QTextDocument;

class Estimator;
class ProjectInfo;

class __attribute__ ((visibility ("default"))) ReleaseInfo: public QObject
{
        Q_OBJECT

    public:
        typedef QHash<QString, ProjectVersion> VersionList;

        enum class State
        {
            Nothing,     //nic się nie dzieje
            Checking,    //sprawdzanie aktualnej wersji
            Downloading, //pobieranie aktualnej wersji
            Building     //budowanie pakietu
        };

        enum class BuildMode
        {
            Normal,      //wywolanie rpmbuild -ba
            Fast         //wywołanie rpmbuild -bi --short-circuit (dla szybkiego poprawiania plików spec)
        };

        enum ChangeType
        {
            StateChange = 1,
            ProgressChange = 2,
            AllChanged = 0xff
        };

    private:
        const QString name;
        bool download;
        bool build;
        int builds;

        qint64 total;   //zmienna uzywana przy pobieraniu danych z internetu - wartość max dla progressBaru
        qint64 done;    //jw, wartość bieżąca

        VersionList localVersions, currentVersions;

        ProjectInfo *projectInfo;   //rodzic
        QString downloadScript;
        Estimator *estimator;
        State state;
        QString downloadedPkg;      //paczka która wlasnie jest pobierania (ma sens tylko dla state==Downloading)

        QString releasePath() const;         //construct path to release
        void setState(State st);

    private slots:
        void setDownloadOption(int);         //używane przez WidgetListItem
        void setBuildOption(int);            //używane przez WidgetListItem
        void updateProgress(int, int);
        void updateProgress(qint64, qint64);

    public:
        explicit ReleaseInfo(const QString &n, ProjectInfo* p);
        virtual ~ReleaseInfo();

        QString getName() const;
        bool getBuildFlag() const;
        bool getDownloadFlag() const;
        QTextDocument *getBuildMesages();
        QString getDownloadScriptFile() const;          //zwraca scieżkę do pliku lua
        QString getSpecFile() const;
        QString getReleasePath() const;                 //release path (dir)
        const QString &getDownloadedPkg() const;
        const ProjectInfo *getProjectInfo() const;
        const VersionList *getLocalVersions() const;    //zwraca wskaźnik na wersję projektu na dysku
        const VersionList *getCurrentVersions() const;  //zwraca wskaźnik na wersję projektu sprawdzoną w internecie
        qint64 getProgressTotal() const;
        qint64 getProgressDone() const;
        void update();                                  //check, what is the newest version
        void downloadPkg();                             //pobierz paczkę z bieżącą wersją
        void buildStarted();
        void buildStopped();
        const Estimator *getEstimator() const;

        State getState() const;

    signals:
        void statusChanged(int);         //coś się zmieniło (progress, lub stan  (pobieranie/budowanie)), sygnał informuje otoczenie że powinno się odswieżyć
        void optionsChanged();           //zmieniły się opcje releasea (buduj/nie buduj, ściągaj/nie ściągaj)
        void logWillChange();            //log budowania zaraz zmieni swoją zawartość.
};

#endif // RELEASEINFO_HPP
