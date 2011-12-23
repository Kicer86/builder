
#include <QLabel>

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


void SpecConstansDialog::addConstant(const QString &name, const QString &value)
{
    QLabel *nameWidget = new QLabel(name);
    QLabel *valueWidget = new QLabel(value);

    ui->constantLayout->addWidget(nameWidget);
    ui->valueLayout->addWidget(valueWidget);
}
