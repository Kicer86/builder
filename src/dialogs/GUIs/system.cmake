

function(GuiFiles list)
  if("${GUI_TYPE}" STREQUAL "KDE4")

	find_package(KDE4 4.0 REQUIRED)
	add_definitions(${KDE4_DEFINITIONS})
	include_directories(${KDE4_INCLUDE_DIR})

	set(EXTRA_LIBS ${KDE4_KIO_LIBRARY} PARENT_SCOPE)
	set(${list} src/dialogs/GUIs/Kde4/openwithdialog.cpp PARENT_SCOPE)

	message(STATUS "Adding Kde4 related files: " ${${list}} )

  elseif("${GUI_TYPE}" STREQUAL "WINDOWS")

	  #nothing extra to do
	  message(STATUS "Adding Windows related files: " ${${list}} )

  else()

	  message(FATAL_ERROR, "Wrong GUI_TYPE: \"${GUI_TYPE}\"")

  endif("${GUI_TYPE}" STREQUAL "KDE4")

endfunction(GuiFiles)
