
#include "data_containers/releaseinfo.hpp"
#include "data_containers/projectinfo.hpp"

#include "releasecopydialog.hpp"
#include "ui_releasecopydialog.h"

ReleaseCopyDialog::ReleaseCopyDialog(const ReleaseInfo &releaseInfo, QWidget *parent):
    QDialog(parent),
    ui(new Ui::ReleaseCopyDialog)
{
    ui->setupUi(this);

    ui->releaseName->setText(releaseInfo.getName());
    ui->projectName->setText(releaseInfo.getProjectInfo()->getName());
}

ReleaseCopyDialog::~ReleaseCopyDialog()
{
    delete ui;
}
