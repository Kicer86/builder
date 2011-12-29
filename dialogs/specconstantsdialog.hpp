#ifndef SPECCONSTANSDIALOG_HPP
#define SPECCONSTANSDIALOG_HPP

#include <utility>
#include <vector>

#include <QDialog>
#include <QLabel>

class QLineEdit;

namespace Ui {
class SpecConstansDialog;
}


class SpecConstantsDialog: public QDialog
{
    Q_OBJECT

    Ui::SpecConstansDialog *ui;

    typedef std::pair<QLineEdit *, QLineEdit *> Variable;   //name and value
    typedef std::vector<Variable> VariableList;

    VariableList variableList;

    private slots:
    void manageVariables();

public:
    explicit SpecConstantsDialog(QWidget *parent = 0);
    ~SpecConstantsDialog();

    void addConstant(const QString &name, const QString &value);
    void addVariable(const QString &name, const QString &value);
    void addSeparator();

    void exec();
};

#endif // SPECCONSTANSDIALOG_HPP
