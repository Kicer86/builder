#include <QApplication>
#include <QTextCodec>

#include "builder.hpp"

#include "misc/settings.hpp"
#include "dialogs/setup.hpp"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));

    QCoreApplication::setOrganizationName("Michał Walenciak");
    QCoreApplication::setOrganizationDomain("http://kicer.sileman.net.pl");
    QCoreApplication::setApplicationName("Builder");

    bool config_ok = true;
    int err = 0;

    if (Settings::instance()->configIsFine() == false)
    {
        Setup setup;
        if (setup.exec() == QDialog::Accepted)
        {
            //apply settings
            const QString projsDir = setup.readProjectsDir();

            Settings::instance()->setProjsPath(projsDir);
            Settings::instance()->setConfigFine();
        }
        else
            config_ok = false;
    }

    if (config_ok)
    {
        Builder builder;
        builder.show();

        err = app.exec();
    }

    return err;
}
