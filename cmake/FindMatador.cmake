# - Find MATADOR
# Find the MATADOR includes and client library
# This module defines
#  MATADOR_INCLUDE_DIR, where to find matador/object/object_store.hpp
#  MATADOR_LIBRARIES, the libraries needed to use MATADOR.
#  MATADOR_FOUND, If false, do not try to use MATADOR.

if(MATADOR_INCLUDE_DIR AND MATADOR_LIBRARIES)
   set(MATADOR_FOUND TRUE)

else()

  find_path(MATADOR_INCLUDE_DIR matador/object/object_store.hpp
      /usr/include/
      /usr/local/include/
      /usr/include/matador/
      /usr/local/include/matador/
      /opt/matador/include/
      $ENV{ProgramFiles}/MATADOR/*/include
      $ENV{SystemDrive}/MATADOR/*/include
  )

  if(WIN32)
    find_library(MATADOR_LIBRARIES NAMES matador-object matador-utils matador-unit matador-orm matador-sql
      PATHS
      $ENV{ProgramFiles}/MATADOR/*/lib
      $ENV{SystemDrive}/MATADOR/*/lib
    )
  else()
    find_library(MATADOR_LIBRARIES NAMES matador-object
      PATHS
      /usr/lib
      /usr/lib/matador
      /usr/local/lib
      /usr/local/lib/matador
      /opt/matador/lib
    )
  endif()

  if(MATADOR_INCLUDE_DIR AND MATADOR_LIBRARIES)
    set(MATADOR_FOUND TRUE)
    message(STATUS "Found MATADOR: ${MATADOR_INCLUDE_DIR}, ${MATADOR_LIBRARIES}")
  else()
    set(MATADOR_FOUND FALSE)
    if (MATADOR_FIND_REQUIRED)
      message(FATAL_ERROR "MATADOR not found.")
    else ()
      message(STATUS "MATADOR not found.")
    endif ()
  endif()

  mark_as_advanced(MATADOR_INCLUDE_DIR MATADOR_LIBRARIES)

endif()
