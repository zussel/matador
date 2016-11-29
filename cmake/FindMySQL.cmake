# - Find mysqlclient
# Find the native MySQL includes and library
#
#  MYSQL_INCLUDE_DIR - where to find mysql.h, etc.
#  MYSQL_LIBRARIES   - List of libraries when using MySQL.
#  MYSQL_FOUND       - True if MySQL found.

IF (MYSQL_INCLUDE_DIR)
  # Already in cache, be silent
  SET(MYSQL_FIND_QUIETLY TRUE)
ENDIF (MYSQL_INCLUDE_DIR)

if(WIN32)
   find_path(MYSQL_INCLUDE_DIR mysql.h
      PATHS
      $ENV{MYSQL_INCLUDE_DIR}
      $ENV{MYSQL_DIR}/include
      $ENV{ProgramFiles}/MySQL/*/include
      $ENV{SystemDrive}/MySQL/*/include
      $ENV{ProgramW6432}/MySQL/*/include
   )
else(WIN32)
   find_path(MYSQL_INCLUDE_DIR mysql.h
      PATHS
      $ENV{MYSQL_INCLUDE_DIR}
      $ENV{MYSQL_DIR}/include
      /usr/local/mysql/include
      /opt/mysql/mysql/include
      PATH_SUFFIXES
      mysql
   )
endif(WIN32)

if(WIN32)
  if (${CMAKE_BUILD_TYPE})
    STRING(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_TOLOWER)
  endif()

  # path suffix for debug/release mode
  # binary_dist: mysql binary distribution
  # build_dist: custom build
  if(CMAKE_BUILD_TYPE_TOLOWER MATCHES "debug")
    SET(binary_dist debug)
    SET(build_dist Debug)
  else(CMAKE_BUILD_TYPE_TOLOWER MATCHES "debug")
    ADD_DEFINITIONS(-DDBUG_OFF)
    SET(binary_dist opt)
    SET(build_dist Release)
  endif(CMAKE_BUILD_TYPE_TOLOWER MATCHES "debug")

  FIND_LIBRARY(MYSQL_LIBRARY NAMES libmysql
    PATHS
    $ENV{MYSQL_DIR}/lib/${binary_dist}
      $ENV{MYSQL_DIR}/libmysql/${build_dist}
      $ENV{MYSQL_DIR}/client/${build_dist}
      $ENV{ProgramFiles}/MySQL/*/lib/${binary_dist}
      $ENV{SystemDrive}/MySQL/*/lib/${binary_dist}
      $ENV{MYSQL_DIR}/lib/opt
      $ENV{MYSQL_DIR}/client/release
      $ENV{ProgramFiles}/MySQL/*/lib/opt
      $ENV{SystemDrive}/MySQL/*/lib/opt
      $ENV{ProgramW6432}/MySQL/*/lib
  )
else(WIN32)
  find_library(MYSQL_LIBRARY NAMES libmysql
    PATHS
    $ENV{MYSQL_DIR}/libmysql_r/.libs
    $ENV{MYSQL_DIR}/lib
    $ENV{MYSQL_DIR}/lib/mysql
    /usr/local/mysql/lib
    /opt/mysql/mysql/lib
    PATH_SUFFIXES 
    mysql
  )
endif(WIN32)

if(WIN32)
#  set(MYSQL_LIB_PATHS
#    $ENV{MYSQL_DIR}/lib/opt
#    $ENV{MYSQL_DIR}/client/release
#    $ENV{ProgramFiles}/MySQL/*/lib/opt
#    $ENV{SystemDrive}/MySQL/*/lib/opt
#  )
#  find_library(MYSQL_LIBRARY NAMES mysqlclient
#    PATHS
#    ${MYSQL_LIB_PATHS}
#  )
else(WIN32)
  set(MYSQL_LIB_PATHS
    $ENV{MYSQL_DIR}/libmysql_r/.libs
    $ENV{MYSQL_DIR}/lib
    $ENV{MYSQL_DIR}/lib/mysql
    /usr/local/mysql/lib
    /opt/mysql/mysql/lib
    PATH_SUFFIXES
    mysql
  )
  find_library(MYSQL_LIBRARY NAMES mysqlclient
    PATHS
    ${MYSQL_LIB_PATHS}
  )
endif(WIN32)

IF (MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)
  SET(MYSQL_FOUND TRUE)
  SET( MYSQL_LIBRARIES ${MYSQL_LIBRARY} )
ELSE (MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)
  SET(MYSQL_FOUND FALSE)
  SET( MYSQL_LIBRARIES )
ENDIF (MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)

IF (MYSQL_FOUND)
#  IF (NOT MYSQL_FIND_QUIETLY)
    MESSAGE(STATUS "Found MySQL LIB: ${MYSQL_LIBRARY}")
    MESSAGE(STATUS "Found MySQL Include: ${MYSQL_INCLUDE_DIR}")
#  ENDIF (NOT MYSQL_FIND_QUIETLY)
ELSE (MYSQL_FOUND)
  IF (MYSQL_FIND_REQUIRED)
#    MESSAGE(STATUS "Looked for MySQL libraries named ${MYSQL_NAMES}.")
#    MESSAGE(FATAL_ERROR "Could NOT find MySQL library")
  ENDIF (MYSQL_FIND_REQUIRED)
ENDIF (MYSQL_FOUND)

MARK_AS_ADVANCED(
  MYSQL_LIBRARY
  MYSQL_INCLUDE_DIR
  )
