#ifndef NEWPROJECTWIZARD_HPP
#define NEWPROJECTWIZARD_HPP

#include <memory>

#include <QWizard>

class QLineEdit;
namespace Ui
{
    class NewProjectWizard;
}

class NewProjectWizard : public QWizard
{
        Q_OBJECT

        std::auto_ptr<Ui::NewProjectWizard> ui;
        QList<QLineEdit *> edits;

        void addReleseLineEdit();

    private slots:
        void textChanged(const QString &);

    public:
        explicit NewProjectWizard(QWidget *parent = 0);
        ~NewProjectWizard();

        QString getProjectName() const;
        QStringList getReleasesNames() const;
};

#endif // NEWPROJECTWIZARD_HPP
