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

#include <QObject>
#include <QStringList>
#include <QHash>

class QProcess;
class QTextDocument;
class QLayout;

class ReleaseInfo;

class BuildProcess: public QObject
{
  Q_OBJECT

  QProcess *process;
  QTextDocument *log;
  ReleaseInfo *releaseInfo;

private slots:
  void read() const;
  void close() const;

public:
  friend class BuildPlugin;
  
  BuildProcess();
  virtual ~BuildProcess();
  
  void stop() const;
  
signals:
  void removeBuildProcess(const ReleaseInfo *) const;
};

class BuildPlugin: public QObject                //it's QObject here, becouse plugin system requires it
{
    Q_OBJECT
    
  public:
    BuildPlugin(const char *);
    virtual ~BuildPlugin();

    typedef QHash<ReleaseInfo*, BuildProcess*> BuildsInfo;

    QString getBuilderName() const;               //return builder name
    virtual QLayout *getBuildButtons() const=0;   //return layout with button(s) for managing build process

  protected:
    void addBuildProcess(const BuildProcess &);   //add *AND* run build process. BuildPlugin takes ownership on BuildProcess
    BuildProcess *findBuildProcess(const ReleaseInfo*);

      protected slots:
    void removeBuildProcess(const ReleaseInfo*);  //remove build process for ReleaseInfo

  private:
    BuildPlugin(const BuildPlugin& other);
    void operator=(const BuildPlugin&);

    const char *name;

    BuildsInfo buildsInfo;
};

Q_DECLARE_INTERFACE(BuildPlugin, "kicer.builder.BuildPlugin/1.0")

#endif // BUILDPLUGIN_HPP
