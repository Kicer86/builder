project(builder)
cmake_minimum_required(VERSION 2.6)
find_package(Qt4 4.6 REQUIRED core gui new)
find_package(Lua51 REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR} .)

if (NOT BUILDER_DATADIR)
	set (BUILDER_DATADIR ${CMAKE_INSTALL_PREFIX}/share/builder)
endif (NOT BUILDER_DATADIR)
if (NOT BUILDER_LIBDIR)
	set (BUILDER_LIBDIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})
endif (NOT BUILDER_LIBDIR)
if (NOT BUILDER_BINDIR)
	set (BUILDER_BINDIR ${CMAKE_INSTALL_PREFIX}/bin)
endif (NOT BUILDER_BINDIR)

configure_file (${CMAKE_SOURCE_DIR}/builder-config.h.cmake ${CMAKE_BINARY_DIR}/builder-config.h)

set(builder_SRCS
  builder.cpp
  main.cpp
  data_containers/editorsmanager.cpp
  data_containers/projectinfo.cpp 
  data_containers/projectsmanager.cpp
  data_containers/projectversion.cpp
  data_containers/releaseinfo.cpp
  dialogs/configdialog.cpp
  misc/downloader.cpp
  misc/editfile.cpp
  misc/estimator.cpp
  misc/funcions.cpp
  misc/sandboxprocess.cpp
  misc/settings.cpp
  misc/wgetwrapper.cpp
  widgets/projectinfowidget.cpp
  widgets/widgetlist/widgetlist.cpp
  widgets/widgetlist/widgetdelegate.cpp
  widgets/widgetlist/widgetlistitem.cpp
  widgets/widgetlist/widgetlistproxymodel.cpp
)

set(builder_MOC
  builder.hpp
  data_containers/editorsmanager.hpp
  data_containers/releaseinfo.hpp
  dialogs/configdialog.hpp
  misc/downloader.hpp
  misc/editfile.hpp
  misc/estimator.hpp
  misc/wgetwrapper.hpp
  widgets/projectinfowidget.hpp
  widgets/widgetlist/widgetlist.hpp
)

set(builder_UIS dialogs/configdialog.ui widgets/projectinfowidget.ui)

qt4_wrap_ui(builder_UIFILES ${builder_UIS})
qt4_automoc(${builder_SRCS})
qt4_wrap_cpp(builder_MOCFILES ${builder_MOC})

add_executable(builder ${builder_SRCS} ${builder_MOCFILES} ${builder_UIFILES})

target_link_libraries(builder 
      ${QT_QTCORE_LIBRARY} 
      ${QT_QTGUI_LIBRARY} 
      ${QT_QTNETWORK_LIBRARY} 
      ${LUA_LIBRARIES}
      htmlParser)

#templates
install(FILES data_files/spec_template DESTINATION ${BUILDER_DATADIR})
    
#target
install (TARGETS builder RUNTIME DESTINATION ${BUILDER_BINDIR})
