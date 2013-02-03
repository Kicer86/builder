project(buildin_plugins)
cmake_minimum_required(VERSION 2.6)

#some definitions
include(${CMAKE_SOURCE_DIR}/src/plugins)

#RPM BUILD
if(UNIX)  #available only on Linux
    include_directories(${QT_INCLUDES})

    set(SRCS rpmbuildplugin.cpp)
    set(HEADERS rpmbuildplugin.hpp)

    qt4_automoc(${SRCS})
    qt4_wrap_cpp(MOCS ${HEADERS})

    add_library(RPMbuild_plugin MODULE ${SRCS} ${MOCS})

    target_link_libraries(RPMbuild_plugin ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

    install(TARGETS RPMbuild_plugin LIBRARY DESTINATION ${BUILDER_DATADIR}/plugins)

    set_target_properties(RPMbuild_plugin PROPERTIES COMPILE_FLAGS ${LIB_COMPILE_FLAGS}
                                                     LINK_FLAGS ${LIB_LINK_FLAGS}
                                                     COMPILE_FLAGS_DEBUG "-Weffc++")
endif(UNIX)
