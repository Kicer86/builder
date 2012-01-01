project(builder)
cmake_minimum_required(VERSION 2.6)
find_package(Qt4 4.6 REQUIRED QtGui QtCore QtNetwork)
find_package(Lua51 REQUIRED)
find_program(INKSCAPE inkscape)

if (INKSCAPE-NOTFOUND)
	message ( FATAL_ERROR "inkscape: not found" )
else (INKSCAPE-NOTFOUND)
	message ( STATUS "inkscape: found" )
endif (INKSCAPE-NOTFOUND)

if (CMAKE_COMPILER_IS_GNUCXX)
  #check version
  string(REPLACE " " ";" C_ARGS "${CMAKE_C_COMPILER_ARG1}")
  execute_process(COMMAND ${CMAKE_C_COMPILER} ${C_ARGS} -dumpversion
				  OUTPUT_VARIABLE GCC_VERSION)
  if (GCC_VERSION VERSION_LESS 4.6)
	message ( FATAL_ERROR "GCC version >= 4.6 required due to c++0x11 standard usage" )
  else(GCC_VERSION VERSION_LESS 4.6)
	message ( STATUS "Acceptable version of gcc found: ${GCC_VERSION}")
  endif(GCC_VERSION VERSION_LESS 4.6)
endif (CMAKE_COMPILER_IS_GNUCXX)

include_directories(${QT_INCLUDES} ${LUA_INCLUDE_DIR} ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR} .)

set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/plugins)  #cmake modules for plugins

add_definitions(-DDEBUG_QSTRING_SUPPORT)      #let debug functions from my library know QString

if (NOT BUILDER_DATADIR)
	set (BUILDER_DATADIR ${CMAKE_INSTALL_PREFIX}/share/builder)
endif (NOT BUILDER_DATADIR)
if (NOT BUILDER_LIBDIR)
	set (BUILDER_LIBDIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})
endif (NOT BUILDER_LIBDIR)
if (NOT BUILDER_BINDIR)
	set (BUILDER_BINDIR ${CMAKE_INSTALL_PREFIX}/bin)
endif (NOT BUILDER_BINDIR)

include (plugins/plugins.cmake)

#pozostałe katalogi po powyższym, ponieważ używają ścieżek
add_subdirectory(images)
add_subdirectory(plugins/buildin)

configure_file (${CMAKE_SOURCE_DIR}/builder-config.h.cmake ${CMAKE_BINARY_DIR}/builder-config.h)

set(builder_SRCS
  builder.cpp
  main.cpp
  data_containers/editorsmanager.cpp
  data_containers/imagesmanager.cpp
  data_containers/projectinfo.cpp
  data_containers/projectsmanager.cpp
  data_containers/projectversion.cpp
  data_containers/releaseinfo.cpp
  dialogs/configdialog.cpp
  dialogs/newprojectwizard.cpp 
  dialogs/releasecopydialog.cpp
  dialogs/progressdialog.cpp
  dialogs/specconstantsdialog.cpp
  misc/broadcast.cpp
  misc/downloader.cpp
  misc/editfile.cpp
  misc/estimator.cpp
  misc/functions.cpp
  misc/sandboxprocess.cpp
  misc/settings.cpp
  misc/wgetwrapper.cpp
  plugins/buildplugin.cpp
  widgets/imagewidget.cpp
  widgets/projectinfowidget.cpp
  widgets/widgetlist/widgetdelegate.cpp
  widgets/widgetlist/widgetlistitem.cpp
  widgets/widgetlist/widgetlistproxymodel.cpp
  widgets/widgetlist/widgetlistview.cpp
)

set(builder_MOC
  builder.hpp
  data_containers/editorsmanager.hpp
  data_containers/projectinfo.hpp
  data_containers/releaseinfo.hpp
  dialogs/configdialog.hpp
  dialogs/newprojectwizard.hpp
  dialogs/releasecopydialog.hpp
  dialogs/progressdialog.hpp
  dialogs/specconstantsdialog.hpp
  misc/broadcast.hpp
  misc/downloader.hpp
  misc/editfile.hpp
  misc/estimator.hpp
  misc/wgetwrapper.hpp
  plugins/buildplugin.hpp
  widgets/imagewidget.hpp
  widgets/projectinfowidget.hpp
  widgets/widgetlist/widgetlistitem.hpp
  widgets/widgetlist/widgetlistview.hpp
)

set(builder_UIS
  dialogs/configdialog.ui
  dialogs/newprojectwizard.ui
  dialogs/releasecopydialog.ui
  dialogs/progressdialog.ui
  dialogs/specconstantsdialog.ui
  widgets/projectinfowidget.ui
)

set(builder_IMGS
  icons/build.svg
  icons/download.svg
  icons/off.svg
  icons/progress.svg
)

qt4_wrap_ui(builder_UIFILES ${builder_UIS})
qt4_automoc(${builder_SRCS})
qt4_wrap_cpp(builder_MOCFILES ${builder_MOC})

add_executable(builder ${builder_SRCS} ${builder_MOCFILES} ${builder_UIFILES})
# ADD_PRECOMPILED_HEADER(builder stdafx.h)
set_target_properties(builder PROPERTIES COMPILE_FLAGS "-std=c++0x"
                                         COMPILE_FLAGS_DEBUG "-Weffc++")

target_link_libraries(builder
      ${QT_QTCORE_LIBRARY}
      ${QT_QTGUI_LIBRARY}
      ${QT_QTNETWORK_LIBRARY}
      ${QT_QTSVG_LIBRARY}
      ${LUA_LIBRARIES}
      htmlParser)

#templates
install(FILES data_files/spec_template DESTINATION ${BUILDER_DATADIR})

#target
install(TARGETS builder RUNTIME DESTINATION ${BUILDER_BINDIR})
