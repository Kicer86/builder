
#ifndef SYSTEMDIALOGS_HPP
#define SYSTEMDIALOGS_HPP

class OpenWithDialog;

class SystemDialogs
{
    protected:
        SystemDialogs() {}
        virtual ~SystemDialogs() {}

    public:
        static SystemDialogs* getInstance();

        virtual OpenWithDialog* getOpenWithDialog();
};

#endif // SYSTEMDIALOGS_HPP
