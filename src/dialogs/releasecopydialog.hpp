#ifndef RELEASECOPYDIALOG_HPP
#define RELEASECOPYDIALOG_HPP

#include <memory>

#include <QDialog>

class ReleaseInfo;

namespace Ui {
class ReleaseCopyDialog;
}

class ReleaseCopyDialog : public QDialog
{
    Q_OBJECT

    std::auto_ptr<Ui::ReleaseCopyDialog> ui;

public:
    explicit ReleaseCopyDialog(const ReleaseInfo &, QWidget *parent = 0);
    ~ReleaseCopyDialog();

    QString getNewName() const;
};

#endif // RELEASECOPYDIALOG_HPP
