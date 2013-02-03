
if (WIN32)  # for windows (dll = runtime)
  set(_MODULE RUNTIME)
else (WIN32)
  set(_MODULE LIBRARY)
endif(WIN32)

#library flags -> hide all symbol, except forced ones
set(LIB_COMPILE_FLAGS "-std=c++0x -fvisibility=hidden -fvisibility-inlines-hidden")

#allow undefined symbols (some will be taken from builder)
set(LIB_LINK_FLAGS "-Wl,--allow-shlib-undefined")
