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

class ReleaseInfo:public QObject
{
    Q_OBJECT

  public:
    typedef QHash<QString, ProjectVersion> VersionList;

    enum State
    {
      Nothing,     //nic się nie dzieje
      Checking,    //sprawdzanie aktualnej wersji
      Downloading, //pobieranie aktualnej wersji
      Building     //budowanie pakietu
    };

    enum BuildMode
    {
      Normal,      //wywolanie rpmbuild -ba
      Fast,        //wywołanie rpmbuild -bi --short-circuit (dla szybkiego poprawiania plików spec)
    };

    enum ChangeType
    {
      StateChange=1,
      ProgressChange=2,
      AllChanged=0xff
    };

  private:
    const int id;        //id wydania
    const QString name;
    bool download;
    bool build;

    qint64 total;   //zmienna uzywana przy pobieraniu danych z internetu - wartość max dla progressBaru
    qint64 done;    //jw, wartość bieżąca

    VersionList localVersions, currentVersions;

    ProjectInfo *projectInfo;   //rodzic
    QString downloadScript;
    QProcess *buildProcess;
    QTextDocument *buildingLog;
    Estimator *estimator;
    State state;
    QString downloadedPkg;      //paczka która wlasnie jest pobierania (ma sens tylko dla state==Downloading)

    QString releasePath() const;
    void appendTextToLog(const QString &msg);
    void setState(State st);

  private slots:
    void downloadCheck(int);         //używane przez WidgetListItem
    void buildCheck(int);            //używane przez WidgetListItem
    void updateProgress(int, int);
    void updateProgress(qint64, qint64);
    void buildMessages();
    void buildFinished( int exitCode, QProcess::ExitStatus exitStatus );
    void buildError(QProcess::ProcessError error );

  public:
    explicit ReleaseInfo(const QString &n, ProjectInfo* p);
    virtual ~ReleaseInfo();

    int getId() const;
    QString getName() const;
    bool getBuildFlag() const;
    bool getDownloadFlag() const;
    QTextDocument *getBuildMesages();
    QString getDownloadScriptFile() const;         //zwraca scieżkę do pliku lua
    QString getSpecFile() const;
    const QString &getDownloadedPkg() const;
    const ProjectInfo *getProjectInfo() const;
    const VersionList *getLocalVersions() const;    //zwraca wskaźnik na wersję projektu na dysku
    const VersionList *getCurrentVersions() const;  //zwraca wskaźnik na wersję projektu sprawdzoną w internecie
    qint64 getProgressTotal() const;
    qint64 getProgressDone() const;
    void update();                                  //sprawdź jakie jest bieżąca wersja
    void downloadPkg();                             //pobierz paczkę z bieżącą wersją
    void buildPkg(BuildMode);      //zbuduj paczkę RPM (lub zatrzym budowę, jeśli wywołane w trakcie budowy)
    const Estimator *getEstimator() const;

    State getState() const;

  signals:
    void changed(int);
};

#endif // RELEASEINFO_HPP
