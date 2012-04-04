# Copyright (C) 2012 IITRBCS
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if (COMPILE_PLUGIN_LIBRARY)
  prepare_carrier(bayer_carrier TYPE BayerCarrier INCLUDE BayerCarrier.h)
endif (COMPILE_PLUGIN_LIBRARY)

if (NOT SKIP_bayer_carrier)

  set(DC1394_FOUND FALSE)
  set(DC1394_SRC libdc1394_bayer.c)
  if (NOT WIN32)
    find_library(lib_raw1394 raw1394)
    find_library(lib_dc1394 dc1394)
    if (lib_raw1394 AND lib_dc1394)
      set(DC1394_FOUND TRUE)
      set(DC1394_LIBRARIES ${lib_raw1394} ${lib_dc1394})
    endif ()
  endif ()

  option(USE_LIBDC1394 "Link DC1394 externally" ${DC1394_FOUND})
  
  if (USE_LIBDC1394)
    add_definitions(-DUSE_LIBDC1394)
    set(DC1394_SRC)
  endif ()

  find_package(YARP REQUIRED)
  include_directories(${YARP_INCLUDE_DIRS})
  add_library(bayer_carrier BayerCarrier.h BayerCarrier.cpp ${DC1394_SRC})
  target_link_libraries(bayer_carrier ${YARP_LIBRARIES})
  if (USE_LIBDC1394)
    target_link_libraries(bayer_carrier ${DC1394_LIBRARIES})
  endif()
endif ()