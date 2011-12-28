
#include <QLabel>
#include <QScrollBar>

#include "specconstantsdialog.hpp"
#include "ui_specconstantsdialog.h"

SpecConstantsDialog::SpecConstantsDialog(QWidget *p) :
    QDialog(p),
    ui(new Ui::SpecConstansDialog)
{
    ui->setupUi(this);

    //turn on synchro between scrolareas
    connect(ui->constansScrollArea->verticalScrollBar(), SIGNAL(valueChanged(int)),
            ui->valuesScrollArea->verticalScrollBar(), SLOT(setValue(int)));

    connect(ui->valuesScrollArea->verticalScrollBar(), SIGNAL(valueChanged(int)),
            ui->constansScrollArea->verticalScrollBar(), SLOT(setValue(int)));
}


SpecConstantsDialog::~SpecConstantsDialog()
{
    delete ui;
}


void SpecConstantsDialog::addConstant(const QString &name, const QString &value)
{
    QLabel *nameWidget = new QLabel(name);
    nameWidget->setTextInteractionFlags(Qt::TextSelectableByMouse);

    QLabel *valueWidget = new QLabel(value);
    valueWidget->setTextInteractionFlags(Qt::TextSelectableByMouse);

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
