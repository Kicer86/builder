
#include "setup.hpp"

#include <QFileDialog>

#include "ui_setup.h"

#include <qtext_choosefile.hpp>

setup::setup(QWidget *p): QWizard(p), ui(new Ui::setup)
{
    ui->setupUi(this);

    QFileDialog *dialog = new QFileDialog();
    dialog->setFileMode(QFileDialog::Directory);
    dialog->setAcceptMode(QFileDialog::AcceptOpen);

    chooseFile = new QtExtChooseFile(ui->page1_button, ui->page1_dirEdit, dialog);
}


setup::~setup()
{
    delete ui;
    delete chooseFile;
}
