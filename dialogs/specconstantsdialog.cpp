
#include <QLabel>

#include "specconstantsdialog.hpp"
#include "ui_specconstantsdialog.h"

SpecConstantsDialog::SpecConstantsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SpecConstansDialog)
{
    ui->setupUi(this);
}

SpecConstantsDialog::~SpecConstantsDialog()
{
    delete ui;
}


void SpecConstantsDialog::addConstant(const QString &name, const QString &value)
{
    QLabel *nameWidget = new QLabel(name);
    QLabel *valueWidget = new QLabel(value);

    ui->constantsLayout->addWidget(nameWidget);
    ui->valuesLayout->addWidget(valueWidget);
}


void SpecConstantsDialog::exec()
{
    //finish list
    ui->constantsLayout->addStretch(1);
    ui->valuesLayout->addStretch(1);

    QDialog::exec();
}
