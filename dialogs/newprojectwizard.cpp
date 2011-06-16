#include "newprojectwizard.hpp"
#include "ui_newprojectwizard.h"

NewProjectWizard::NewProjectWizard(QWidget *p) :
    QWizard(p),
    ui(new Ui::NewProjectWizard)
{
    ui->setupUi(this);
}

NewProjectWizard::~NewProjectWizard()
{
    delete ui;
}
