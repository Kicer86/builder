#ifndef SPECCONSTANSDIALOG_HPP
#define SPECCONSTANSDIALOG_HPP

#include <QDialog>

namespace Ui {
class SpecConstansDialog;
}

class SpecConstansDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SpecConstansDialog(QWidget *parent = 0);
    ~SpecConstansDialog();
    
private:
    Ui::SpecConstansDialog *ui;
};

#endif // SPECCONSTANSDIALOG_HPP
