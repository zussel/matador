# Find MATADOR Backend MySQL
# Find the MATADOR MySQL backend library
# This module defines
#  MATADOR_BACKEND_MYSQL_LIBRARY, the library needed to use MATADOR with MySQL.
#  MATADOR_BACKEND_MYSQL_FOUND, If false, do not try to use MATADOR.

if(MATADOR_BACKEND_MYSQL_LIBRARY)
   set(MATADOR_BACKEND_MYSQL_FOUND TRUE)
else()

  if(WIN32)
    find_library(MATADOR_BACKEND_MYSQL_LIBRARY matador-mysql
      PATHS
      $ENV{ProgramFiles}/matador/*/lib/
      ${PROGRAMFILES}/matador/lib/
      $ENV{SystemDrive}/matador/*/lib/
    )      
  else()
    find_library(MATADOR_BACKEND_MYSQL_LIBRARY matador-mysql
      PATHS
      /usr/lib
      /usr/lib/matador
      /usr/local/lib
      /usr/local/lib/matador
      /opt/matador/lib
    )
  endif()

  if(MATADOR_BACKEND_MYSQL_LIBRARY)
    set(MATADOR_BACKEND_MYSQL_FOUND TRUE)
  else()
    set(MATADOR_BACKEND_MYSQL_FOUND FALSE)
    if (MATADOR_FIND_REQUIRED)
      message(FATAL_ERROR "MATADOR MySQL backend not found.")
    else ()
      message(STATUS "MATADOR MySQL backend not found.")
    endif ()
  endif()

  mark_as_advanced(MATADOR_BACKEND_MYSQL_LIBRARY)

endif()
