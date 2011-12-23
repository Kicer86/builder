#ifndef SPECCONSTANSDIALOG_HPP
#define SPECCONSTANSDIALOG_HPP

#include <QDialog>

namespace Ui {
class SpecConstansDialog;
}

class SpecConstansDialog : public QDialog
{
    Q_OBJECT


    Ui::SpecConstansDialog *ui;

public:
    explicit SpecConstansDialog(QWidget *parent = 0);
    ~SpecConstansDialog();

    void addConstant(const QString &name, const QString &value);
};

#endif // SPECCONSTANSDIALOG_HPP
