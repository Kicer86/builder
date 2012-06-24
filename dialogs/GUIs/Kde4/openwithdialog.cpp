
#include "dialogs/openwithdialog.hpp"

#include <kopenwithdialog.h>
#include <qtext_choosefile.hpp>


class OpenWithDialog::Impl: public KOpenWithDialog, public QtExtChooseFileDialog
{
    public:
        virtual int exec()
        {
            return KOpenWithDialog::exec();
        }

        virtual QString result() const
        {
            KService::Ptr kservice = KOpenWithDialog::service();

            const QString program = kservice->exec();

            return program;
        }
};



OpenWithDialog::OpenWithDialog(): m_impl(new Impl())
{

}


OpenWithDialog::~OpenWithDialog()
{

}


int OpenWithDialog::exec()
{
    return m_impl->exec();
}


QString OpenWithDialog::result() const
{
    return m_impl->result();
}
