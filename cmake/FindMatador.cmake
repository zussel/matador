# - Find MATADOR
# Find the MATADOR includes and client library
# This module defines
#  MATADOR_INCLUDE_DIR, where to find matador/object/object_store.hpp
#  MATADOR_LIBRARIES, the libraries needed to use MATADOR.
#  MATADOR_FOUND, If false, do not try to use MATADOR.

if(MATADOR_INCLUDE_DIR AND MATADOR_LIBRARIES)
   set(MATADOR_FOUND TRUE)

else()

  set(MATADOR_MODULES
    object
    utils
    unit
    sql
    orm
  )

  set(PROGRAMFILES $ENV{ProgramFiles})
  if (DEFINED ENV{ProgramW6432})
    set(PROGRAMFILES $ENV{ProgramW6432})
  endif()

  find_path(MATADOR_INCLUDE_DIR matador/object/object_store.hpp
      /usr/include/
      /usr/local/include/
      /usr/include/matador/
      /usr/local/include/matador/
      /opt/matador/include/
      $ENV{ProgramFiles}/matador/*/include/
      ${PROGRAMFILES}/matador/include/
      $ENV{SystemDrive}/matador/*/include/
  )

  foreach(MATADOR_MODULE ${MATADOR_MODULES})
    if(WIN32)

      STRING(TOUPPER ${MATADOR_MODULE} MATADOR_MODULE_UPPER)

      find_library(MATADOR_${MATADOR_MODULE_UPPER}_LIBRARY matador-${MATADOR_MODULE}
        PATHS
        $ENV{ProgramFiles}/matador/*/lib/
        ${PROGRAMFILES}/matador/lib/
        $ENV{SystemDrive}/matador/*/lib/
      )      
    else()
      find_library(MATADOR_${MATADOR_MODULE_UPPER}_LIBRARY matador-${MATADOR_MODULE}
        PATHS
        /usr/lib
        /usr/lib/matador
        /usr/local/lib
        /usr/local/lib/matador
        /opt/matador/lib
      )
    endif()

    if (MATADOR_${MATADOR_MODULE_UPPER}_LIBRARY)
      set(MATADOR_LIBRARY_FOUND TRUE)
    else()
      set(MATADOR_LIBRARY_FOUND FALSE)
      break()
    endif()
  endforeach(MATADOR_MODULE)

  if(MATADOR_INCLUDE_DIR AND MATADOR_LIBRARY_FOUND)
    set(MATADOR_FOUND TRUE)
  else()
    set(MATADOR_FOUND FALSE)
    if (MATADOR_FIND_REQUIRED)
      message(FATAL_ERROR "MATADOR not found.")
    else ()
      message(STATUS "MATADOR not found.")
    endif ()
  endif()

  mark_as_advanced(MATADOR_INCLUDE_DIR)
  foreach(MATADOR_MODULE ${MATADOR_MODULES})
    STRING(TOUPPER ${MATADOR_MODULE} MATADOR_MODULE_UPPER)
    LIST(APPEND MATADOR_LIBRARIES ${MATADOR_${MATADOR_MODULE_UPPER}_LIBRARY})
    mark_as_advanced(MATADOR_${MATADOR_MODULE_UPPER}_LIBRARY)
  endforeach()
  mark_as_advanced(MATADOR_LIBRARIES)

endif()
