
IF (ACE_DEBUG_FOUND)
 LINK_LIBRARIES(optimized ${ACE_LIBRARY} debug ${ACE_DEBUG_LIBRARY})
 SET(ACE_LIB "optimized ${ACE_LIBRARY} debug ${ACE_DEBUG_LIBRARY}" CACHE INTERNAL "libraries")
ELSE (ACE_DEBUG_FOUND)
 LINK_LIBRARIES(${ACE_LIBRARY})
 SET(ACE_LIB "${ACE_LIBRARY}" CACHE INTERNAL "libraries")
ENDIF (ACE_DEBUG_FOUND)

IF (WIN32 AND NOT CYGWIN)
  LINK_LIBRARIES(winmm)
  SET(ACE_LIB "${ACE_LIB} winmm" CACHE INTERNAL "libraries")
ENDIF (WIN32 AND NOT CYGWIN)

