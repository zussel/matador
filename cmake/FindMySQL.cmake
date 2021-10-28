# - Find mysqlclient
# Find the native MySQL includes and library
#
#  MYSQL_INCLUDE_DIR - where to find mysql.h, etc.
#  MYSQL_LIBRARIES   - List of libraries when using MySQL.
#  MYSQL_FOUND       - True if MySQL found.

IF (MYSQL_INCLUDE_DIR)
  # Already in cache, be silent
  SET(MYSQL_FIND_QUIETLY TRUE)
  MESSAGE(STATUS "MYSQL_INCLUDE_DIR ${MYSQL_INCLUDE_DIR}")
ENDIF (MYSQL_INCLUDE_DIR)

if(WIN32)
  MESSAGE(STATUS "MYSQL_DIR $ENV{MYSQL_DIR}")

  MESSAGE(STATUS "Checking $ENV{MYSQL_DIR}/include/mysql.h")
  if(EXISTS "$ENV{MYSQL_DIR}/include/mysql.h")
    MESSAGE(STATUS "MySQL header exists")
  else()
    MESSAGE(STATUS "MySQL header doesn't exists")
  endif()

  MESSAGE(STATUS "Checking $ENV{MYSQL_DIR}/lib/libmysql.dll")
  if(EXISTS "$ENV{MYSQL_DIR}/lib/libmysql.dll")
    MESSAGE(STATUS "MySQL lib exists")
  else()
    MESSAGE(STATUS "MySQL lib doesn't exists")
  endif()

endif(WIN32)

if(WIN32)
  MESSAGE(STATUS "Searching for MySQL include in $ENV{ProgramFiles}")
  find_path(MYSQL_INCLUDE_DIR mysql.h
    PATHS
#    $ENV{MYSQL_INCLUDE_DIR}
    $ENV{MYSQL_DIR}/include
    $ENV{ProgramFiles}/MySQL/*/include
#    $ENV{SystemDrive}/MySQL/*/include
#    $ENV{ProgramW6432}/MySQL/*/include
#    $ENV{ProgramFiles}/MariaDB/*/include
#    $ENV{SystemDrive}/MariaDB/*/include
#    $ENV{ProgramW6432}/MariaDB/*/include
   )
  MESSAGE(STATUS "Found MYSQL_INCLUDE_DIR ${MYSQL_INCLUDE_DIR}")
else(WIN32)
  find_path(MYSQL_INCLUDE_DIR mysql.h
    PATHS
    $ENV{MYSQL_DIR}/include
    $ENV{MYSQL_INCLUDE_DIR}
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

  FIND_LIBRARY(MYSQL_LIBRARY NAMES libmysql libmariadb
    PATHS
    $ENV{MYSQL_DIR}/lib
#    $ENV{MYSQL_DIR}/lib/${binary_dist}
#    $ENV{MYSQL_DIR}/libmysql/${build_dist}
#    $ENV{MYSQL_DIR}/client/${build_dist}
#    $ENV{ProgramFiles}/MySQL/*/lib/${binary_dist}
    $ENV{ProgramFiles}/MySQL/*/lib
#    $ENV{SystemDrive}/MySQL/*/lib/${binary_dist}
#    $ENV{ProgramFiles}/MariaDB/*/lib/${binary_dist}
#    $ENV{SystemDrive}/MariaDB/*/lib/${binary_dist}
#    $ENV{MYSQL_DIR}/lib/opt
#    $ENV{MYSQL_DIR}/client/release
#    $ENV{ProgramFiles}/MySQL/*/lib/opt
#    $ENV{SystemDrive}/MySQL/*/lib/opt
#    $ENV{ProgramW6432}/MySQL/*/lib
#    $ENV{ProgramFiles}/MariaDB/*/lib/opt
#    $ENV{SystemDrive}/MariaDB/*/lib/opt
#    $ENV{ProgramW6432}/MariaDB/*/lib
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

IF (MYSQL_INCLUDE_DIR)
  MESSAGE(STATUS "MariaDB include ${MYSQL_INCLUDE_DIR}")
ELSE (MYSQL_INCLUDE_DIR)
  MESSAGE(STATUS "MariaDB include dir not found")
ENDIF (MYSQL_INCLUDE_DIR)

IF (MYSQL_LIBRARIES)
  MESSAGE(STATUS "MariaDB libs ${MYSQL_LIBRARIES}")
ELSE (MYSQL_LIBRARIES)
  MESSAGE(STATUS "MariaDB libs dir not found")
ENDIF (MYSQL_LIBRARIES)


IF (MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)
  SET(MYSQL_FOUND TRUE)
  SET( MYSQL_LIBRARIES ${MYSQL_LIBRARY} )
ELSE (MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)
  SET(MYSQL_FOUND FALSE)
  SET( MYSQL_LIBRARIES )
ENDIF (MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)

MARK_AS_ADVANCED(MYSQL_LIBRARY MYSQL_INCLUDE_DIR)
