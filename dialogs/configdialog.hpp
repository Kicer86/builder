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


#ifndef CONFIGDIALOG_HPP
#define CONFIGDIALOG_HPP

#include <memory>

#include <QDialog>

#include "misc/settings.hpp"

namespace Ui
{
    class ConfigDialog;
}

class ConfigDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit ConfigDialog(QWidget* p=0, Qt::WindowFlags f=0);
        ~ConfigDialog();

        Settings::EnvType getEnvType() const;
        QString getExtEnvPath() const;
        QString getProjsPath() const;
        QStringList getExtList() const;

    private:
        std::auto_ptr<Ui::ConfigDialog> ui;

    private slots:
        void locEnvButtonPressed();
        void extEnvButtonPressed();
        void envBrowsePressed();
        void projsBrowsePressed();
};

#endif // CONFIGDIALOG_HPP
