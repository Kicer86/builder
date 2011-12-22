#ifndef RELEASECOPYDIALOG_HPP
#define RELEASECOPYDIALOG_HPP

#include <QDialog>

namespace Ui {
class ReleaseCopyDialog;
}

class ReleaseCopyDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ReleaseCopyDialog(QWidget *parent = 0);
    ~ReleaseCopyDialog();
    
private:
    Ui::ReleaseCopyDialog *ui;
};

#endif // RELEASECOPYDIALOG_HPP
