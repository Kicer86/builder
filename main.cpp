#include <QApplication>
#include <QTextCodec>

#include "builder.hpp"

int main(int argc, char** argv)
{
  QApplication app(argc, argv);
  QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));
  QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf8"));
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));

  QCoreApplication::setOrganizationName("Michał Walenciak");
  QCoreApplication::setOrganizationDomain("http://kicer.sileman.net.pl");
  QCoreApplication::setApplicationName("Builder");

  Builder builder;
  builder.show();
  
  return app.exec();
}
