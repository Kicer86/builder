/*
    Interface for building plugins
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


#ifndef BUILDPLUGIN_HPP
#define BUILDPLUGIN_HPP

#include <map>

#include <QObject>
#include <QStringList>

class QProcess;
class QTextDocument;
class QLayout;

class ReleaseInfo;

class BuildProcess: public QObject
{
        Q_OBJECT

        QProcess *process;              //build process
        QTextDocument *log;             //build log
        ReleaseInfo *releaseInfo;       //pointer to related ReleaseInfo
        int progress;                   //build progress (if -1 then unknown)

        void appendToLog(const QString& str) const;

    private slots:
        void read() const;
        void close(int);

    public:
        friend class BuildPlugin;

        BuildProcess(ReleaseInfo *);
        virtual ~BuildProcess();

        void stop() const;              //terminate build process
        QProcess *getProcess()
        {
            return process;
        }

        QTextDocument *getLog()
        {
            return log;
        }

    signals:
        void removeBuildProcess(ReleaseInfo *);
};


class BuildPlugin: public QObject                //it's QObject here, becouse plugin system requires it
{
        Q_OBJECT

    public:
        BuildPlugin(const char *);
        virtual ~BuildPlugin();

        typedef std::map<ReleaseInfo*, BuildProcess*> BuildsInfo;

        const QString& getBuilderName() const;        //return builder name
        virtual QLayout* getBuildButtons() const = 0; //return layout with button(s) for managing build process
        virtual QWidget* getBuildLog() const = 0;     //return widget with build messages
        virtual void updateTab() = 0;                 //function called, when tab with build logs should be updated
        virtual void updateProgress(int) = 0;         //set build progress (-1 means that progress is unknown)

    protected:
        //add *AND* run build process. BuildPlugin takes ownership on BuildProcess
        void addBuildProcess(const QString &, const QStringList &, BuildProcess *);
        BuildProcess *findBuildProcess(ReleaseInfo *releaseInfo);

    protected slots:
        void removeBuildProcess(ReleaseInfo *);  //remove build process for ReleaseInfo

    private:
        BuildPlugin(const BuildPlugin& other);
        void operator=(const BuildPlugin&);

        const QString name;

        BuildsInfo buildsInfo;
};

Q_DECLARE_INTERFACE(BuildPlugin, "kicer.builder.BuildPlugin/1.0")

#endif // BUILDPLUGIN_HPP
