#ifndef SETUP_HPP
#define SETUP_HPP

#include <QWizard>

namespace Ui {
    class setup;
}

class QtExtChooseFile;

class Setup : public QWizard
{
        Q_OBJECT

        Ui::setup *ui;
        QtExtChooseFile *chooseFile;

    public:
        explicit Setup(QWidget *parent = 0);
        virtual ~Setup();

        QString readProjectsDir() const;
};

#endif // SETUP_HPP
