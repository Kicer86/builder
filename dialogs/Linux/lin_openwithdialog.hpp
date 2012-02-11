
#if defined GUI_TYPE_KDE4
    #include <kopenwithdialog.h>


    typedef KOpenWithDialog OpenWithDialog;

#elif defined GUI_TYPE_GNOME


#else
    #error wrong gui choise
#endif
