
if (WIN32)  # for windows (dll = runtime)
  set(_MODULE RUNTIME)
else (WIN32)
  set(_MODULE LIBRARY)
endif(WIN32)
