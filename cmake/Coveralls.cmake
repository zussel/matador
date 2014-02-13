# Check prereqs
FIND_PROGRAM( GCOV_PATH gcov )

IF(NOT GCOV_PATH)
	MESSAGE(FATAL_ERROR "gcov not found! Aborting...")
ELSE()
  MESSAGE(STATUS "Found Gcov")
ENDIF() # NOT GCOV_PATH

find_package(Git)

if(NOT GIT_FOUND)
     message(FATAL_ERROR "git not found")
endif()

IF(NOT CMAKE_COMPILER_IS_GNUCXX)
	# Clang version 3.0.0 and greater now supports gcov as well.
	MESSAGE(WARNING "Compiler is not GNU gcc! Clang Version 3.0.0 and greater supports gcov as well, but older versions don't.")
	
	IF(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		MESSAGE(FATAL_ERROR "Compiler is not GNU gcc! Aborting...")
	ENDIF()
ENDIF() # NOT CMAKE_COMPILER_IS_GNUCXX

IF ( NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
  MESSAGE( WARNING "Code coverage results with an optimized (non-Debug) build may be misleading" )
ENDIF() # NOT CMAKE_BUILD_TYPE STREQUAL "Debug"

FUNCTION(SETUP_TARGET_FOR_COVERALLS _targetname _testrunner _sourcedir)

  message(STATUS "setting up coveralls target")

  set(COVERALLS_SOURCE_DIR ${_sourcedir})

  configure_file(${CMAKE_SOURCE_DIR}/scripts/coveralls.sh.in ${CMAKE_BINARY_DIR}/coveralls-upload @ONLY)

  # add target for coverage by gcov
  ADD_CUSTOM_TARGET(${_targetname}
    # Run tests
		COMMAND ${_testrunner} ${ARGV3}
  )

ENDFUNCTION()
