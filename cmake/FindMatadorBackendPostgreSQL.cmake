# Find MATADOR Backend PostgreSQL
# Find the MATADOR postgresql backend library
# This module defines
#  MATADOR_BACKEND_POSTGRESQL_LIBRARY, the library needed to use MATADOR with postgresql.
#  MATADOR_BACKEND_POSTGRESQL_FOUND, If false, do not try to use MATADOR.

if(MATADOR_BACKEND_POSTGRESQL_LIBRARY)
   set(MATADOR_BACKEND_POSTGRESQL_FOUND TRUE)
else()

  if(WIN32)
    find_library(MATADOR_BACKEND_POSTGRESQL_LIBRARY matador-postgresql
      PATHS
      $ENV{ProgramFiles}/matador/*/lib/
      ${PROGRAMFILES}/matador/lib/
      $ENV{SystemDrive}/matador/*/lib/
    )      
  else()
    find_library(MATADOR_BACKEND_POSTGRESQL_LIBRARY matador-postgresql
      PATHS
      /usr/lib
      /usr/lib/matador
      /usr/local/lib
      /usr/local/lib/matador
      /opt/matador/lib
    )
  endif()

  if(MATADOR_BACKEND_POSTGRESQL_LIBRARY)
    set(MATADOR_BACKEND_POSTGRESQL_FOUND TRUE)
  else()
    set(MATADOR_BACKEND_POSTGRESQL_FOUND FALSE)
    if (MATADOR_FIND_REQUIRED)
      message(FATAL_ERROR "MATADOR PostgreSQL backend not found.")
    else ()
      message(STATUS "MATADOR PostgreSQL backend not found.")
    endif ()
  endif()

  mark_as_advanced(MATADOR_BACKEND_POSTGRESQL_LIBRARY)

endif()
