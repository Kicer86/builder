#include "specconstansdialog.hpp"
#include "ui_specconstansdialog.h"

SpecConstansDialog::SpecConstansDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SpecConstansDialog)
{
    ui->setupUi(this);
}

SpecConstansDialog::~SpecConstansDialog()
{
    delete ui;
}
