
#ifndef WIN_OPENWITHDIALOG_HPP
#define WIN_OPENWITHDIALOG_HPP


class OpenWithDialog: public QtExtChooseFileDialog
{
    public:
        virtual int exec()
        {
            return 1;
        }

        virtual QString result() const
        {
            return "test";
        }
};


#endif
