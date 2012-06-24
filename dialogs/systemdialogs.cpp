
#include "systemdialogs.hpp"

#include "openwithdialog.hpp"


OpenWithDialog* SystemDialogs::getOpenWithDialog()
{
    return new OpenWithDialog();
}


SystemDialogs* SystemDialogs::getInstance()
{
    static SystemDialogs systemDialogs;
    return &systemDialogs;
}
