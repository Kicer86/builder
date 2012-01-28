 
#translations generator (just for convenience) - target translations is added to make

set(translations pl)
foreach(tr ${translations})
  set (tr_file ${CMAKE_SOURCE_DIR}/tr/builder_${tr}.ts)
  add_custom_command(
        OUTPUT ${tr}             #this file won't be created. Just a trick to force make to always run this command
        COMMAND lupdate 
        ARGS -noobsolete -verbose ${CMAKE_SOURCE_DIR} -ts ${tr_file}
      )
      
endforeach(tr)

add_custom_target(translations DEPENDS ${translations})
