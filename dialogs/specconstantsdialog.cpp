
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


void SpecConstantsDialog::addSeparator()
{
    QFrame *line1 = new QFrame();
    line1->setFrameShape(QFrame::HLine);
    line1->setFrameShadow(QFrame::Sunken);

    QFrame *line2 = new QFrame();
    line2->setFrameShape(QFrame::HLine);
    line2->setFrameShadow(QFrame::Sunken);

    ui->constantsLayout->addWidget(line1);
    ui->valuesLayout->addWidget(line2);
}

void SpecConstantsDialog::exec()
{
    //finish list
    ui->constantsLayout->addStretch(1);
    ui->valuesLayout->addStretch(1);

    QDialog::exec();
}
