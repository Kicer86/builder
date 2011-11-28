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


#ifndef PROJECTVERSION_HPP
#define PROJECTVERSION_HPP

#include <QUrl>
#include <QFileInfo>

class QSettings;

struct ProjectVersion
{
        //status of data
        enum class Status
        {
            Empty,             //no data provided
            Filled,            //data filled
            Error              //there was an error while filling data (ie server error, path no found etc)
        };

        ProjectVersion(const QUrl &url);
        ProjectVersion(const QString &filePart);
        ProjectVersion();

        void setLocalFile(const QFileInfo &file);
        void setName(const QString &n);
        QFileInfo getLocalFile() const;
        QString text() const;  //zwraca to, co podano jako parametr konstruktora
        QString getVersion() const;
        QString getExtension() const;
        QString getName() const;
        const QString& getErrorMsg() const
        {
            return statusError;
        }

        QUrl getPkgUrl() const;
        void setPkgUrl(const QUrl& url);
        Status getStatus() const;
        bool save(QSettings* settings) const;  //zapisz ustawienia
        void load(QSettings *);                //wczytaj ustawienia
        void setError(const QString &msg)      //mark 'this' as invalid and set error message
        {
            status = Status::Error;
            statusError = msg;
        }

        bool operator>(const ProjectVersion &pV) const;
        bool operator<(const ProjectVersion &pV) const;
        bool operator==(const ProjectVersion &pV) const;
        bool operator!=(const ProjectVersion &pV) const;

    private:
        Status status;
        QString statusError;   //is status == Status::Error, this variable contains information about it

        int majorN;
        int minorN;
        int release;
        int build;
        int len;         //długość (liczba elementów) wersji (1: x; 2:x.y; 3: x.y.z; 4: x.y.z.t)
        QString phase;   //beta, alpha etc
        QString ext;     //rozszerzenie
        QUrl url;        //url to package
        QString relativePath; //scieżka do pliku na dysku (względem katalogu z projektami)
        QString rawVersion;   //wersja taka jaką pobraliśmy z netu, nieobrobiona
        QString name;    /** Nazwa (człon przed wersją) pliku.
                             Serwowana jest przez skrypt lua, więc użytkownik może sobie nadać dowolną.
                             Używane do rozrózniania kilku paczek w obrębie tego samego projektu (w tym do uzupełniania pól w plikach spec).
                             Jedna z paczek powinna mieć nazwę taką jak nazwa projektu. Potrzeby aby działało
                             autouzupełnianie pliku spec \see ProjectInfoWidget::specButtonPressed() */

        int compare(const ProjectVersion &pV) const;
        int extCompare(const ProjectVersion &pV) const;

        void setValues(const QString &p);
};


#endif // PROJECTVERSION_HPP
