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

#include <QObject>

#include "plugins/buildplugin.hpp"

class QPushButton;
class QGridLayout;
class QProgressBar;

class RpmBuildPlugin: public BuildPlugin
{
    Q_OBJECT
    Q_INTERFACES(BuildPlugin)

    RpmBuildPlugin(const RpmBuildPlugin&);
    void operator=(const RpmBuildPlugin&);
    
    enum Type
    {
      Normal, 
      Fast
    };

    QGridLayout  *buttons;
    QPushButton  *buildButton;
    QPushButton  *fastBuildButton;
    QProgressBar *progressBar;
    
    void build(Type);

  private slots:
    void buildButtonPressed();
    void fastBuildButtonPressed();

  public:
    RpmBuildPlugin();
    virtual ~RpmBuildPlugin();

    virtual QLayout* getBuildButtons() const;
    virtual void updateProgress(int);
};

#endif // RPMBUILDPLUGIN_HPP
