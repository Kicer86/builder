#ifndef RELEASECOPYDIALOG_HPP
#define RELEASECOPYDIALOG_HPP

#include <QDialog>

class ReleaseInfo;

namespace Ui {
class ReleaseCopyDialog;
}

class ReleaseCopyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReleaseCopyDialog(const ReleaseInfo &, QWidget *parent = 0);
    ~ReleaseCopyDialog();

private:
    Ui::ReleaseCopyDialog *ui;
};

#endif // RELEASECOPYDIALOG_HPP
