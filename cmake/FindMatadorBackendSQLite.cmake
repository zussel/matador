# Find MATADOR Backend SQLite
# Find the MATADOR sqlite backend library
# This module defines
#  MATADOR_BACKEND_SQLITE_LIBRARY, the library needed to use MATADOR with sqlite.
#  MATADOR_BACKEND_SQLITE_FOUND, If false, do not try to use MATADOR.

if(MATADOR_BACKEND_SQLITE_LIBRARY)
   set(MATADOR_BACKEND_SQLITE_FOUND TRUE)
else()

  if(WIN32)
    find_library(MATADOR_BACKEND_SQLITE_LIBRARY matador-sqlite
      PATHS
      $ENV{ProgramFiles}/matador/*/lib/
      ${PROGRAMFILES}/matador/lib/
      $ENV{SystemDrive}/matador/*/lib/
    )      
  else()
    find_library(MATADOR_BACKEND_SQLITE_LIBRARY matador-sqlite
      PATHS
      /usr/lib
      /usr/lib/matador
      /usr/local/lib
      /usr/local/lib/matador
      /opt/matador/lib
    )
  endif()

  if(MATADOR_BACKEND_SQLITE_LIBRARY)
    set(MATADOR_BACKEND_SQLITE_FOUND TRUE)
  else()
    set(MATADOR_BACKEND_SQLITE_FOUND FALSE)
    if (MATADOR_FIND_REQUIRED)
      message(FATAL_ERROR "MATADOR SQLite backend not found.")
    else ()
      message(STATUS "MATADOR SQLite backend not found.")
    endif ()
  endif()

  mark_as_advanced(MATADOR_BACKEND_SQLITE_LIBRARY)

endif()
