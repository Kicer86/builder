#ifndef NEWPROJECTWIZARD_HPP
#define NEWPROJECTWIZARD_HPP

#include <QWizard>

class QLineEdit;
namespace Ui
{
  class NewProjectWizard;
}

class NewProjectWizard : public QWizard
{
    Q_OBJECT

    Ui::NewProjectWizard *ui;
    QList<QLineEdit *> edits;
    
    void addReleseLineEdit();
    
  private slots:
    void textChanged(const QString &);
    
  public:
    explicit NewProjectWizard(QWidget *parent = 0);
    ~NewProjectWizard();

};

#endif // NEWPROJECTWIZARD_HPP
