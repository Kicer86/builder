/*
    Rpm building plugin
    Copyright (C) 2011  Michał Walenciak <Kicer86@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#ifndef RPMBUILDPLUGIN_HPP
#define RPMBUILDPLUGIN_HPP

#include <utility>

#include <QObject>

#include "plugins/buildplugin.hpp"

class QGridLayout;
class QPlainTextEdit;
class QPushButton;
class QExProgressBarView;

class __attribute__ ((visibility ("default"))) RpmBuildPlugin: public BuildPlugin
{
        Q_OBJECT
        Q_INTERFACES(BuildPlugin)

        bool buttonsEnabled;

        RpmBuildPlugin(const RpmBuildPlugin&);
        void operator=(const RpmBuildPlugin&);

        enum Type
        {
            Normal,
            Fast
        };

        QGridLayout  *buttons;
        QPlainTextEdit *log;
        QPushButton  *buildButton;
        QPushButton  *fastBuildButton;
        QPushButton  *editSpecButton;
        QPushButton  *showMacrosButton;
        QPushButton  *showConstantsButton;
        QExProgressBarView *progressBar;

        void build(Type);
        virtual void updateTab();

        typedef std::pair<QString, QString> Pair;
        typedef QList<Pair> List;
        typedef QHash<QString, QString> Hash;

        QString replaceVariables(const QString &, const Hash &) const;   //replace variables/constants in str by theirs values
        List getListOfConstants(const ReleaseInfo *) const;
        List getListOfVariables(const ReleaseInfo *) const;
        Hash solveVariables(const List &variables, const List &constants) const;
        void refreshGui();              //another releaseInfo selected, or build started, buttons have to be refreshed

    private slots:
        void buildButtonPressed();
        void fastBuildButtonPressed();

        void specButtonPressed();
        void specConstantsButtonPressed();
        void showMacrosButtonPressed();

    protected:
        virtual void newReleaseInfoSelected();
        virtual void buildProcessStopped(ReleaseInfo *);

    public:
        RpmBuildPlugin();
        virtual ~RpmBuildPlugin();

        virtual QLayout* getBuildButtons() const;
        virtual QWidget* getBuildLog() const;
};

#endif // RPMBUILDPLUGIN_HPP
