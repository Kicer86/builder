#ifndef SPECCONSTANSDIALOG_HPP
#define SPECCONSTANSDIALOG_HPP

#include <QDialog>

namespace Ui {
class SpecConstansDialog;
}

class SpecConstantsDialog: public QDialog
{
    Q_OBJECT

    Ui::SpecConstansDialog *ui;

public:
    explicit SpecConstantsDialog(QWidget *parent = 0);
    ~SpecConstantsDialog();

    void addConstant(const QString &name, const QString &value);

    void exec();
};

#endif // SPECCONSTANSDIALOG_HPP
