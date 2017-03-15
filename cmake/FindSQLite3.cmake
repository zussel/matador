# - Try to find SQLite3
# Once done this will define
#  SQLITE3_FOUND - System has SQLite3
#  SQLITE3_INCLUDE_DIR - The SQLite3 include directories
#  SQLITE3_LIBRARY - The libraries needed to use SQLite3

IF (WIN32)
	FIND_PATH( SQLITE3_INCLUDE_DIR sqlite3.h
		$ENV{PROGRAMFILES}/SQLite/include
		DOC "The directory where sqlite3.h resides"
	)

	FIND_LIBRARY( SQLITE3_LIBRARY
		NAMES sqlite3
		PATHS
		$ENV{PROGRAMFILES}/SQLite/lib
		DOC "The SQLite3 library"
	)
ELSE (WIN32)
	FIND_PATH( SQLITE3_INCLUDE_DIR sqlite3.h
		/usr/include
		/usr/local/include
		/sw/include
		/opt/local/include
		DOC "The directory where sqlite3.h resides")
	FIND_LIBRARY( SQLITE3_LIBRARY
		NAMES sqlite3
		PATHS
		/usr/lib64
		/usr/lib
		/usr/local/lib64
		/usr/local/lib
		/sw/lib
		/opt/local/lib
		DOC "The SQLite3 library")
ENDIF (WIN32)

IF (SQLITE3_INCLUDE_DIR AND SQLITE3_LIBRARY)
	SET( SQLITE3_FOUND 1 CACHE STRING "Set to 1 if SQLite3 is found, 0 otherwise")
	MESSAGE(STATUS "SQLite3 found")
ELSE (SQLITE3_INCLUDE_DIR AND SQLITE3_LIBRARY)
	SET( SQLITE3_FOUND 0 CACHE STRING "Set to 1 if SQLite3 is found, 0 otherwise")
	MESSAGE(STATUS "SQLite3 not found")
ENDIF (SQLITE3_INCLUDE_DIR AND SQLITE3_LIBRARY)

MARK_AS_ADVANCED( SQLITE3_FOUND )
