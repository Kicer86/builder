#ifndef SETUP_HPP
#define SETUP_HPP

#include <QWizard>

namespace Ui {
    class setup;
}

class QtExtChooseFile;

class setup : public QWizard
{
        Q_OBJECT

        Ui::setup *ui;
        QtExtChooseFile *chooseFile;

    public:
        explicit setup(QWidget *parent = 0);
        ~setup();
};

#endif // SETUP_HPP
