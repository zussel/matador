# Find MATADOR Backend MSSQL
# Find the MATADOR MSSQL backend library
# This module defines
#  MATADOR_BACKEND_MSSQL_LIBRARY, the library needed to use MATADOR with MSSQL.
#  MATADOR_BACKEND_MSSQL_FOUND, If false, do not try to use MATADOR.

if(MATADOR_BACKEND_MSSQL_LIBRARY)
   set(MATADOR_BACKEND_MSSQL_FOUND TRUE)
else()

  if(WIN32)
    find_library(MATADOR_BACKEND_MSSQL_LIBRARY matador-mssql
      PATHS
      $ENV{ProgramFiles}/matador/*/lib/
      ${PROGRAMFILES}/matador/lib/
      $ENV{SystemDrive}/matador/*/lib/
    )      
  else()
    find_library(MATADOR_BACKEND_MSSQL_LIBRARY matador-mssql
      PATHS
      /usr/lib
      /usr/lib/matador
      /usr/local/lib
      /usr/local/lib/matador
      /opt/matador/lib
    )
  endif()

  if(MATADOR_BACKEND_MSSQL_LIBRARY)
    set(MATADOR_BACKEND_MSSQL_FOUND TRUE)
  else()
    set(MATADOR_BACKEND_MSSQL_FOUND FALSE)
    if (MATADOR_FIND_REQUIRED)
      message(FATAL_ERROR "MATADOR MSSQL backend not found.")
    else ()
      message(STATUS "MATADOR MSSQL backend not found.")
    endif ()
  endif()

  mark_as_advanced(MATADOR_BACKEND_MSSQL_LIBRARY)

endif()
