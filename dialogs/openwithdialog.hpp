
#ifdef LINUX
    #include "Linux/lin_openwithdialog.hpp"
#elif defined WINDOWS
    #include "Windows/win_openwithdialog.hpp"
#else
    #error "Build platform is not specified (see main CmakeLists.txt)"
#endif
