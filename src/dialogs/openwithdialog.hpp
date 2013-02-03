
#ifndef OPEN_WITH_DIALOG_HPP
#define OPEN_WITH_DIALOG_HPP

#include <memory>

#include <QString>

class OpenWithDialog
{
        class Impl;

        std::unique_ptr<Impl> m_impl;

    public:
        OpenWithDialog();
        virtual ~OpenWithDialog();

        virtual int exec();
        virtual QString result() const;
};


#endif
