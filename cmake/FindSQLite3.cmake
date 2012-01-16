# - Try to find SQLite3
# Once done this will define
#  SQLITE3_FOUND - System has SQLite3
#  SQLITE3_INCLUDE_DIR - The SQLite3 include directories
#  SQLITE3_LIBRARY - The libraries needed to use SQLite3
#  SQLITE3_DEFINITION - Compiler switches required for using SQLite3

IF( SQLITE3_INCLUDE_DIR AND SQLITE3_LIBRARY )
  SET(SQLITE3_FIND_QUIETLY TRUE)
ENDIF( SQLITE3_INCLUDE_DIR AND SQLITE3_LIBRARY )

FIND_PATH( SQLITE3_INCLUDE_DIR sqlite3.h  )

FIND_LIBRARY(SQLITE3_LIBRARY NAMES sqlite3 )

IF( SQLITE3_LIBRARY AND SQLITE3_INCLUDE_DIR )
  SET( SQLITE3_FOUND TRUE )	
ENDIF( SQLITE3_LIBRARY AND SQLITE3_INCLUDE_DIR )

IF( SQLITE3_FOUND )
  IF( NOT SQLITE3_FIND_QUIETLY )	
    MESSAGE( STATUS "Found Sqlite3 header file in ${SQLITE3_INCLUDE_DIR}")
    MESSAGE( STATUS "Found Sqlite3 libraries: ${SQLITE3_LIBRARY}")
  ENDIF( NOT SQLITE3_FIND_QUIETLY )
ELSE(SQLITE3_FOUND)
  IF( SQLITE3_FIND_REQUIRED)
    MESSAGE( FATAL_ERROR "Could not find Sqlite3" )
  ELSE( SQLITE3_FIND_REQUIRED)
    MESSAGE( STATUS "Optional package Sqlite3 was not found" )
  ENDIF( SQLITE3_FIND_REQUIRED)
ENDIF(SQLITE3_FOUND)






#IF (WIN32)
#	FIND_PATH( FOO_INCLUDE_PATH foo/foo.h
#		$ENV{PROGRAMFILES}/foo-0.1/include
#		DOC "The directory where foo/foo.h resides")
#	FIND_LIBRARY( FOO_LIBRARY
#		NAMES foo
#		PATHS
#		$ENV{PROGRAMFILES}/foo-0.1/lib
#		DOC "The Foo library")
#ELSE (WIN32)
#	FIND_PATH( FOO_INCLUDE_PATH foo/foo.h
#		/usr/include
#		/usr/local/include
#		/sw/include
#		/opt/local/include
#		DOC "The directory where foo/foo.h resides")
#	FIND_LIBRARY( FOO_LIBRARY
#		NAMES foo
#		PATHS
#		/usr/lib64
#		/usr/lib
#		/usr/local/lib64
#		/usr/local/lib
#		/sw/lib
#		/opt/local/lib
#		DOC "The Foo library")
#ENDIF (WIN32)
#
#IF (FOO_INCLUDE_PATH)
#	SET( FOO_FOUND 1 CACHE STRING "Set to 1 if Foo is found, 0 otherwise")
#ELSE (FOO_INCLUDE_PATH)
#	SET( FOO_FOUND 0 CACHE STRING "Set to 1 if Foo is found, 0 otherwise")
#ENDIF (FOO_INCLUDE_PATH)
#
#MARK_AS_ADVANCED( FOO_FOUND )

# in cmakelists.txt
#MESSAGE(STATUS "Looking for Foo")
#INCLUDE(FindFoo.cmake)
#IF(FOO_FOUND)
#	MESSAGE(STATUS "Looking for Foo - found")
#ELSE(FOO_FOUND)
#	MESSAGE(FATAL_ERROR "Looking for Foo - not found")
#ENDIF(FOO_FOUND)
