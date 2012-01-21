#include "setup.hpp"
#include "ui_setup.h"

setup::setup(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::setup)
{
    ui->setupUi(this);
}

setup::~setup()
{
    delete ui;
}
