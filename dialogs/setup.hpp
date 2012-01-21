#ifndef SETUP_HPP
#define SETUP_HPP

#include <QWizard>

namespace Ui {
    class setup;
}

class setup : public QWizard
{
        Q_OBJECT
        
    public:
        explicit setup(QWidget *parent = 0);
        ~setup();
        
    private:
        Ui::setup *ui;
};

#endif // SETUP_HPP
