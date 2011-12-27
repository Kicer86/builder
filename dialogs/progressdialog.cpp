#include "progressdialog.hpp"
#include "ui_progressdialog.h"

ProgressDialog::ProgressDialog(QWidget *p) :
    QWidget(p),
    ui(new Ui::ProgressDialog)
{
    ui->setupUi(this);
}


ProgressDialog::~ProgressDialog()
{
    delete ui;
}
