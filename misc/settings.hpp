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


#ifndef SETTINGS_HPP
#define SETTINGS_HPP

class QString;
class QStringList;
class QSettings;

class Settings
{
    Settings();
    ~Settings();

    QSettings *settings;

  public:
    enum EnvType
    {
      Local,
      External
    };

    static Settings *instance();
    QString getProjectsPath() const;
    QString getEnvPath() const;
    EnvType getEnvType() const;
    QStringList getExtList() const;
    QString getEditor() const;
    bool configIsFine() const;

    void setEnvPath(const QString& path);
    void setEnvType(EnvType type);
    void setProjsPath(const QString& path);
    void setExtList(const QStringList& list);
    void setEditor(const QString &);
    void setConfigFine();
};

#endif // SETTINGS_HPP
