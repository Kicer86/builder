#ifndef PROGRESSDIALOG_HPP
#define PROGRESSDIALOG_HPP

#include <memory>

#include <QWidget>

namespace Ui {
    class ProgressDialog;
}

class ProgressDialog : public QWidget
{
        Q_OBJECT

    public:
        explicit ProgressDialog(QWidget *parent = 0);
        ~ProgressDialog();

    private:
        std::auto_ptr<Ui::ProgressDialog> ui;
};

#endif // PROGRESSDIALOG_HPP
