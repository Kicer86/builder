#include <QtGui/QApplication>
#include <QTextCodec>

#include "builder.hpp"


#include <QRegExp>
#include <QDebug>
#include <QTime>

int main(int argc, char** argv)
{
  QApplication app(argc, argv);
  QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));
  QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf8"));
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));

  QCoreApplication::setOrganizationName("Michał Walenciak");
  QCoreApplication::setOrganizationDomain("http://kicer.sileman.net.pl");
  QCoreApplication::setApplicationName("Builder");
  
//   QRegExp version("^\\[([0-9 ]{3})\\%\\].*");
//   qDebug() << version.exactMatch("[  3%] hjhjkhjk");
//   qDebug() << version.captureCount();
//   qDebug() << version.capturedTexts();
//   qDebug() << version.capturedTexts()[1].toInt();
// 
//   return 0;

  builder foo;
  foo.show();
  return app.exec();
}
