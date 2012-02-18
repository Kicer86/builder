project(buildin_plugins)
cmake_minimum_required(VERSION 2.6)

#find_package(Qt4 4.7 REQUIRED QtGui QtCore)

include(plugins)

include_directories(${QT_INCLUDES})

#library flags -> hide all symbol, except forced ones, allow undefined symbols (some will be taken from builder)
set(LIB_COMPILE_FLAGS "-std=c++0x -fvisibility=hidden -fvisibility-inlines-hidden")
set(LIB_LINK_FLAGS "-Wl,--allow-shlib-undefined")

set(SRCS rpmbuildplugin.cpp)
set(HEADERS rpmbuildplugin.hpp)

qt4_automoc(${SRCS})
qt4_wrap_cpp(MOCS ${HEADERS})

add_library(RPMbuild_plugin SHARED ${SRCS} ${MOCS})

target_link_libraries(RPMbuild_plugin ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

install(TARGETS RPMbuild_plugin RUNTIME DESTINATION ${BUILDER_DATADIR}/plugins)

set_target_properties(RPMbuild_plugin PROPERTIES COMPILE_FLAGS ${LIB_COMPILE_FLAGS}
                                                 LINK_FLAGS ${LIB_LINK_FLAGS}
                                                 COMPILE_FLAGS_DEBUG "-Weffc++")
