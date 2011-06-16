#ifndef NEWPROJECTWIZARD_HPP
#define NEWPROJECTWIZARD_HPP

#include <QWizard>

namespace Ui {
    class NewProjectWizard;
}

class NewProjectWizard : public QWizard
{
    Q_OBJECT

public:
    explicit NewProjectWizard(QWidget *parent = 0);
    ~NewProjectWizard();

private:
    Ui::NewProjectWizard *ui;
};

#endif // NEWPROJECTWIZARD_HPP
