project(buildin_plugins)

include(plugins)

set(LIB_COMPILE_FLAGS "-std=c++0x -fvisibility=hidden -fvisibility-inlines-hidden")

set(SRCS rpmbuildplugin.cpp)
set(HEADERS rpmbuildplugin.hpp)

qt4_automoc(${SRCS})
qt4_wrap_cpp(MOCS ${HEADERS})

add_library(RPMbuild_plugin MODULE ${SRCS} ${MOCS})
install(TARGETS RPMbuild_plugin ${_MODULE} DESTINATION ${BUILDER_DATADIR}/plugins)

set_target_properties(RPMbuild_plugin PROPERTIES COMPILE_FLAGS ${LIB_COMPILE_FLAGS}
                                                 COMPILE_FLAGS_DEBUG "-Weffc++")
