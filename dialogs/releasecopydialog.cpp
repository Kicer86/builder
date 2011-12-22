#include "releasecopydialog.hpp"
#include "ui_releasecopydialog.h"

ReleaseCopyDialog::ReleaseCopyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReleaseCopyDialog)
{
    ui->setupUi(this);
}

ReleaseCopyDialog::~ReleaseCopyDialog()
{
    delete ui;
}
