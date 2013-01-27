
project(images)
cmake_minimum_required(VERSION 2.6)

set(builder_IMGS
  build.svg
  download.svg
  off.svg
  progress.svg
)

unset(PROCESSED_IMAGES)
foreach(IMG ${builder_IMGS})
  foreach(RES 16 32 48 64 96 128)

    get_filename_component(IMAGE_NAME_WE ${IMG} NAME_WE)

    set(IMAGE_DIR ${RES}x${RES})
    set(IMAGE_NAME ${IMAGE_DIR}/${IMAGE_NAME_WE}.png)

    if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/${IMAGE_DIR})
      message("Creating directory ${CMAKE_CURRENT_BINARY_DIR}/${IMAGE_DIR} for file ${IMG}")
      file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${IMAGE_DIR} )
    endif(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/${IMAGE_DIR})

    add_custom_command(
        OUTPUT ${IMAGE_NAME}
        COMMAND ${INKSCAPE}
        ARGS --without-gui -w ${RES} -h ${RES} --file=${CMAKE_CURRENT_SOURCE_DIR}/${IMG} --export-png=${CMAKE_CURRENT_BINARY_DIR}/${IMAGE_DIR}/${IMAGE_NAME_WE}.png
        #COMMAND ${CONVERT}
        #ARGS -resize ${RES}x${RES} ${CMAKE_CURRENT_BINARY_DIR}/${IMAGE_DIR}/${IMAGE_NAME_WE}.png ${CMAKE_CURRENT_BINARY_DIR}/${IMAGE_DIR}/${IMAGE_NAME_WE}.png
      )

    set(PROCESSED_IMAGES ${PROCESSED_IMAGES} ${IMAGE_DIR}/${IMAGE_NAME_WE}.png)

    #processed
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${IMAGE_DIR}/${IMAGE_NAME_WE}.png DESTINATION ${BUILDER_DATADIR}/icons/${IMAGE_DIR}/ )
  endforeach(RES)

  #raw
  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${IMG} DESTINATION ${BUILDER_DATADIR}/icons/svg/)
endforeach(IMG)


add_custom_target(images ALL DEPENDS ${PROCESSED_IMAGES})
message(${PROJECT_BINARY_DIR})
