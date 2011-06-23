/*
    Klasa służąca do uruchamiania procesu w określonym środowisku.
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

#include "stdafx.h"

#include "sandboxprocess.hpp"
#include "settings.hpp"

QString SandboxProcess::runProcess(const QString& prcs, const QStringList& args, QProcess* proc)
{
  QString homePath=getenv("HOME");
  if (Settings::instance()->getEnvType()==Settings::External)
    homePath="/root";

  //wymuś HOME w środowisku
  //(fakeroot nie oszukuje $HOME (bo normlanie do /root nie ma dostępu,
  // ale tu uzywany zewnętrznego środowiska w którym taki dostęp jest możliwy))
  QProcessEnvironment environment=QProcessEnvironment::systemEnvironment();
  environment.insert("HOME", homePath);
  environment.insert("TMPDIR", homePath + "/tmp");
  environment.insert("TMP", homePath + "/tmp");
  environment.insert("CFLAGS", QString(""));
  environment.insert("CXXFLAGS", QString(""));
  environment.insert("LDFLAGS", QString("") );
//   environment.insert("CFLAGS", QString("%1 -O3").arg(getenv("OPTFLAGS")) );
//   environment.insert("CXXFLAGS", QString("%1 -O3").arg(getenv("OPTFLAGS")) );
//   environment.insert("LDFLAGS", QString("%1").arg(getenv("LDFLAGS")) );
  proc->setProcessEnvironment(environment);
  
  /*
  + FFLAGS='-O2 -g -pipe -Wformat -Werror=format-security -Wp,-D_FORTIFY_SOURCE=2 -fstack-protector --param=ssp-buffer-size=4'
+ export FFLAGS
+ LDFLAGS='-Wl,--warn-common -flto -Wl,--as-needed -Wl,--no-undefined -Wl,-z,relro -Wl,-O1 -Wl,--build-id -Wl,--enable-new-dtags'
*/
  QStringList arguments;
  QString process;
  
  if (Settings::instance()->getEnvType()==Settings::External)
  {
    process="fakeroot";
    arguments << "fakechroot" << "/usr/sbin/chroot" << decoratePath("") << prcs;
  }
  else
    process=decoratePath(prcs);
  
  arguments << args;
  
  proc->start(process, arguments, QIODevice::ReadOnly);
  
  return QString ("%1 %2").arg(process, arguments.join(" "));
}


QString SandboxProcess::decoratePath(const QString& path)
{
  QString envPath="";
  if (Settings::instance()->getEnvType()==Settings::External)
    envPath=Settings::instance()->getEnvPath() + "/";
  
  return QString("%1%2").arg(envPath).arg(path);
}
