
project(builder)
cmake_minimum_required(VERSION 2.6)
find_package(Qt4 4.6 REQUIRED QtGui QtCore QtNetwork)
find_package(Lua51 REQUIRED)
find_package(OpenLibrary REQUIRED common QtExt htmlParser)
find_program(INKSCAPE inkscape)


#make sure inkscape is installed
if(INKSCAPE-NOTFOUND)
    message( FATAL_ERROR "inkscape: not found" )
else(INKSCAPE-NOTFOUND)
    message( STATUS "inkscape: found" )
endif(INKSCAPE-NOTFOUND)


#make sure, gcc is fresh enought
if(CMAKE_COMPILER_IS_GNUCXX)
  #check version
  string(REPLACE " " ";" C_ARGS "${CMAKE_C_COMPILER_ARG1}")
  execute_process(COMMAND ${CMAKE_C_COMPILER} ${C_ARGS} -dumpversion
                  OUTPUT_VARIABLE GCC_VERSION)
  if(GCC_VERSION VERSION_LESS 4.6)
    message( FATAL_ERROR "GCC version >= 4.6 required due to c++11 standard usage" )
  else(GCC_VERSION VERSION_LESS 4.6)
    message( STATUS "Acceptable version of gcc found: ${GCC_VERSION}")
  endif(GCC_VERSION VERSION_LESS 4.6)
endif(CMAKE_COMPILER_IS_GNUCXX)



#platform specific definitions
if(UNIX)
    add_definitions(-DLINUX)
    set(DEFAULT_GUI_TYPE KDE4)
elseif(WIN32)
    add_definitions(-DWINDOWS)
    set(DEFAULT_GUI_TYPE WINDOWS)
    set(WIN_GUI_TYPE WIN32)
endif(UNIX)


#add some build options/features
set(GUI_TYPE ${DEFAULT_GUI_TYPE} CACHE STRING "Use system related widgets from (possibilities: KDE4, GNOME, WINDOWS)")
add_definitions(-DGUI_TYPE_${GUI_TYPE})


#include directories
include_directories(${QT_INCLUDES} ${LUA_INCLUDE_DIR} ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${OPENLIBRARY_INCLUDES} src)


#set path for modules
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/plugins)  #cmake modules for plugins

add_definitions(-DDEBUG_QSTRING_SUPPORT)      #let debug functions from my library know QString


#set variables with paths
if(NOT BUILDER_DATADIR)
    set(BUILDER_DATADIR ${CMAKE_INSTALL_PREFIX}/share/builder)
endif(NOT BUILDER_DATADIR)

if(NOT BUILDER_LIBDIR)
    set(BUILDER_LIBDIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})
endif(NOT BUILDER_LIBDIR)

if(NOT BUILDER_BINDIR)
    set(BUILDER_BINDIR ${CMAKE_INSTALL_PREFIX}/bin)
endif(NOT BUILDER_BINDIR)

#include plugins
include(src/plugins/plugins.cmake)

#pozostałe katalogi po powyższym, ponieważ używają ścieżek
add_subdirectory(images)
add_subdirectory(tr)
add_subdirectory(src/plugins/buildin)

configure_file(${CMAKE_SOURCE_DIR}/builder-config.h.cmake ${CMAKE_BINARY_DIR}/builder-config.h)


#load system dependent files
include(src/dialogs/GUIs/system.cmake)
set(GuiFiles "")
set(EXTRA_LIBS "")             #additional libs - depens on used gui
GuiFiles(GuiFiles)

message(STATUS ${GuiFiles})

set(builder_SRCS
  src/builder.cpp
  src/main.cpp
  src/data_containers/editorsmanager.cpp
  src/data_containers/imagesmanager.cpp
  src/data_containers/projectinfo.cpp
  src/data_containers/projectsmanager.cpp
  src/data_containers/projectversion.cpp
  src/data_containers/releaseinfo.cpp
  src/dialogs/configdialog.cpp
  src/dialogs/newprojectwizard.cpp
  src/dialogs/releasecopydialog.cpp
  src/dialogs/progressdialog.cpp
  src/dialogs/setup.cpp
  src/dialogs/specconstantsdialog.cpp
  src/dialogs/systemdialogs.cpp
  src/misc/broadcast.cpp
  src/misc/downloader.cpp
  src/misc/editfile.cpp
  src/misc/estimator.cpp
  src/misc/functions.cpp
  src/misc/sandboxprocess.cpp
  src/misc/settings.cpp
  src/misc/wgetwrapper.cpp
  src/plugins/buildplugin.cpp
  src/widgets/imagewidget.cpp
  src/widgets/projectinfowidget.cpp
  src/widgets/widgetlist/widgetdelegate.cpp
  src/widgets/widgetlist/widgetlistitem.cpp
  src/widgets/widgetlist/widgetlistproxymodel.cpp
  src/widgets/widgetlist/widgetlistview.cpp
  src/widgets/widgetlist/items/projectinfoitem.cpp
  src/widgets/widgetlist/items/releaseinfoitem.cpp
  ${GuiFiles}
)


set(builder_MOC
  src/builder.hpp
  src/data_containers/editorsmanager.hpp
  src/data_containers/projectinfo.hpp
  src/data_containers/releaseinfo.hpp
  src/dialogs/configdialog.hpp
  src/dialogs/newprojectwizard.hpp
  src/dialogs/releasecopydialog.hpp
  src/dialogs/progressdialog.hpp
  src/dialogs/setup.hpp
  src/dialogs/specconstantsdialog.hpp
  src/misc/broadcast.hpp
  src/misc/downloader.hpp
  src/misc/editfile.hpp
  src/misc/estimator.hpp
  src/misc/wgetwrapper.hpp
  src/plugins/buildplugin.hpp
  src/widgets/imagewidget.hpp
  src/widgets/projectinfowidget.hpp
  src/widgets/widgetlist/widgetlistitem.hpp
  src/widgets/widgetlist/widgetlistview.hpp
  src/widgets/widgetlist/items/projectinfoitem.hpp
  src/widgets/widgetlist/items/releaseinfoitem.hpp
)

set(builder_UIS
  src/dialogs/configdialog.ui
  src/dialogs/newprojectwizard.ui
  src/dialogs/releasecopydialog.ui
  src/dialogs/progressdialog.ui
  src/dialogs/setup.ui
  src/dialogs/specconstantsdialog.ui
  src/widgets/projectinfowidget.ui
)

set(builder_IMGS
  icons/build.svg
  icons/download.svg
  icons/off.svg
  icons/progress.svg
)

set( builder_TR
  tr/builder_pl.ts
)

qt4_wrap_ui(builder_UIFILES ${builder_UIS})
qt4_automoc(${builder_SRCS})
qt4_wrap_cpp(builder_MOCFILES ${builder_MOC})
qt4_add_translation(builder_QM ${builder_TR})

add_executable(builder ${WIN_GUI_TYPE} ${builder_SRCS} ${builder_MOCFILES} ${builder_UIFILES} ${builder_QM})

set_target_properties(builder PROPERTIES COMPILE_FLAGS "-std=c++11 -fvisibility=hidden -fvisibility-inlines-hidden"
                                         COMPILE_FLAGS_DEBUG "-Weffc++")

target_link_libraries(builder
      ${QT_QTCORE_LIBRARY}
      ${QT_QTGUI_LIBRARY}
      ${QT_QTNETWORK_LIBRARY}
      ${QT_QTSVG_LIBRARY}
      ${EXTRA_LIBS}               #gui specific stuff
      ${LUA_LIBRARIES}
      ${OPENLIBRARY_LIBRARIES}
      )

#templates
install(FILES data_files/spec_template DESTINATION ${BUILDER_DATADIR})

#translations
install(FILES ${builder_QM} DESTINATION ${BUILDER_DATADIR}/translations/)

#target
install(TARGETS builder RUNTIME DESTINATION ${BUILDER_BINDIR})
