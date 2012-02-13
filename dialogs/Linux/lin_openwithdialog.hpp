
#ifndef LIN_OPENWITHDIALOG_HPP
#define LIN_OPENWITHDIALOG_HPP


#if defined GUI_TYPE_KDE4 || 1
    #include <kopenwithdialog.h>
    #include <qtext_choosefile.hpp>

    class OpenWithDialog: public KOpenWithDialog, public QtExtChooseFileDialog
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

#elif defined GUI_TYPE_GNOME


#else
    #error wrong gui choise
#endif


#endif
