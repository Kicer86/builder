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

    Ui::ReleaseCopyDialog *ui;

public:
    explicit ReleaseCopyDialog(const ReleaseInfo &, QWidget *parent = 0);
    ~ReleaseCopyDialog();

    QString getNewName() const;
};

#endif // RELEASECOPYDIALOG_HPP
