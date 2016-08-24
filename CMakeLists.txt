# project name
PROJECT(oos)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# project version
SET(${PROJECT_NAME}_MAJOR_VERSION 0)
SET(${PROJECT_NAME}_MINOR_VERSION 2)
SET(${PROJECT_NAME}_PATCH_LEVEL 1)

IF(CMAKE_CXX_COMPILER MATCHES "clang")
	MESSAGE(STATUS "Clang detected - Adding compiler flags")
	SET(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -pedantic")
ELSEIF(CMAKE_COMPILER_IS_GNUCXX)
    MESSAGE(STATUS "GCC detected - Adding compiler flags")
    
    EXECUTE_PROCESS(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    if (GCC_VERSION VERSION_GREATER 4.6)
        MESSAGE(STATUS "Version > 4.6")
        SET(CMAKE_CXX_FLAGS "-std=c++11 -std=gnu++11 -Wextra -Wall -pedantic")
    else ()
        MESSAGE(STATUS "Version <= 4.6")
        SET(CMAKE_CXX_FLAGS "-std=c++0x -std=gnu++0x -Wextra -Wall -pedantic")
    endif()
ENDIF()

MESSAGE(STATUS "Architecture: ${CMAKE_SYSTEM_PROCESSOR}")
MESSAGE(STATUS "Architecture System: ${CMAKE_SYSTEM_NAME}")


SET(SYSTEM_NAME_LOWER)
STRING(TOLOWER ${CMAKE_SYSTEM_NAME} SYSTEM_NAME_LOWER)

# add module path
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

IF (NOT CMAKE_BUILD_TYPE)
  MESSAGE(STATUS "No build type selected, default to Release")
  SET(CMAKE_BUILD_TYPE "Release")
ELSE()
  MESSAGE(STATUS "Current build type ${CMAKE_BUILD_TYPE}")
ENDIF()

option(COVERALLS "Enable generation and sending of coveralls data" false)
option(COVERAGE "Enable generation of code coverage" false)

SET(GCOV_BINARY "gcov" CACHE STRING "gcov binary")

MESSAGE(STATUS "gcov binary: ${GCOV_BINARY}")

if (NOT MSVC AND (COVERALLS OR COVERAGE) AND CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT CMAKE_CXX_COMPILER MATCHES "clang")
  message(STATUS "coverage for compiler ${CMAKE_CXX_COMPILER}")
  include(CodeCoverage)
  include(Coveralls)
  message(STATUS "will generate coverage data")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall -W -fprofile-arcs -ftest-coverage")
  set(CMAKE_C_FLAGS "{CMAKE_C_FLAGS} -g -O0 -Wall -W -fprofile-arcs -ftest-coverage")
  #SET(CMAKE_SHARED_LINKER_FLAGS="-fprofile-arcs -ftest-coverage")
  set(CMAKE_EXE_LINKER_FLAGS "-fprofile-arcs -ftest-coverage")
  
  if (COVERAGE)
    setup_target_for_coverage(coverage test_oos lib coverage "exec;all")
  endif()
  
  if (COVERALLS)
    setup_target_for_coveralls(coveralls test_oos lib "exec;all")
  endif()
else()
  message(STATUS "no coverage for compiler ${CMAKE_CXX_COMPILER}")
endif()

# Put the libaries and binaries that get built into directories at the
# top of the build tree rather than in hard-to-find leaf
# directories. This simplifies manual testing and the use of the build
# tree rather than installed Boost libraries.
IF (WIN32)
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib)
  SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib)
  SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin)
  SET(CMAKE_MODULE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin)
ELSE()
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  SET(CMAKE_MODULE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
ENDIF()

#
# configure include dir
#
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib/include)

SET(BACKENDS
  SQLite3
  MySQL
  ODBC
)

FOREACH(backend ${BACKENDS})
  STRING(TOUPPER ${backend} BACKEND_UPPER)
  OPTION(OOS_${BACKEND_UPPER} "Enable or disable backend ${backend}" TRUE)
  
  IF (OOS_${BACKEND_UPPER})
    FIND_PACKAGE(${backend})
    IF (${BACKEND_UPPER}_FOUND)
      MESSAGE(STATUS "Found backend ${backend}")
      ADD_DEFINITIONS(-DOOS_${BACKEND_UPPER})
    ELSE()
      MESSAGE(STATUS "No backend ${backend}")
    ENDIF()
  ELSE()
    MESSAGE(STATUS "Backend ${backend} disabled")
  ENDIF()
ENDFOREACH(backend)

IF (SQLITE3_FOUND)
  MESSAGE(STATUS "Adding SQLite3 include directory")
  INCLUDE_DIRECTORIES(${SQLITE3_INCLUDE_DIR})
ELSE ()
  MESSAGE(STATUS "Not adding SQLite3 include directory")
ENDIF ()

IF (MYSQL_FOUND)
  INCLUDE_DIRECTORIES(${MYSQL_INCLUDE_DIR})
ENDIF ()

INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR})

# contains the full path to the top level directory of your build tree 
MESSAGE( STATUS "PROJECT_BINARY_DIR: " ${PROJECT_BINARY_DIR} )

# contains the full path to the root of your project source directory,
# i.e. to the nearest directory where CMakeLists.txt contains the PROJECT() command 
MESSAGE( STATUS "PROJECT_SOURCE_DIR: " ${PROJECT_SOURCE_DIR} )

MESSAGE( STATUS "CMAKE_BINARY_DIR: " ${CMAKE_BINARY_DIR} )

ENABLE_TESTING()

ADD_SUBDIRECTORY(lib)
ADD_SUBDIRECTORY(db)
ADD_SUBDIRECTORY(doc)
ADD_SUBDIRECTORY(test)

#INSTALL(
#	TARGETS oos-tools
#	RUNTIME
#	DESTINATION bin
#	COMPONENT example
#)

INSTALL(
  DIRECTORY ${PROJECT_BINARY_DIR}/doc
  DESTINATION .
  COMPONENT doc
)

INSTALL(
	FILES ${PROJECT_BINARY_DIR}/version.hpp
	DESTINATION include/oos
	COMPONENT headers
)

# Basic CPack configuration   
SET(CPACK_PACKAGE_NAME "oos")
SET(CPACK_PACKAGE_VENDOR "zussel-soft")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "OOS Open Object Store - Store all your objects in one container")
SET(CPACK_PACKAGE_VERSION "0.2.1")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "2")
SET(CPACK_PACKAGE_VERSION_PATCH "1")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "oos")
SET(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_PROCESSOR}-${SYSTEM_NAME_LOWER}")

# Tell CPack all of the components to install. The "ALL"
# refers to the fact that this is the set of components that
# will be included when CPack is instructed to put everything
# into the binary installer (the default behavior).
SET(CPACK_COMPONENTS_ALL libraries headers doc)

# Set the displayed names for each of the components to install.
# These will be displayed in the list of components inside the installer.
SET(CPACK_COMPONENT_APPLICATIONS_DISPLAY_NAME "OOS")
SET(CPACK_COMPONENT_LIBRARIES_DISPLAY_NAME "Libraries")
SET(CPACK_COMPONENT_HEADERS_DISPLAY_NAME "OOS Headers")

# Provide descriptions for each of the components to install.
# When the user hovers the mouse over the name of a component, 
# the description will be shown in the "Description" box in the
# installer. If no descriptions are provided, the "Description" 
# box will be removed.
#SET(CPACK_COMPONENT_APPLICATIONS_DESCRIPTION 
#  "Store all your objects in one container.")
SET(CPACK_COMPONENT_LIBRARIES_DESCRIPTION
  "Store all your objects in one container.")
SET(CPACK_COMPONENT_HEADERS_DESCRIPTION
  "C/C++ header files for OOS")

# Put the components into two different groups: "Runtime" and "Development"
#SET(CPACK_COMPONENT_APPLICATIONS_GROUP "Runtime")
SET(CPACK_COMPONENT_LIBRARIES_GROUP "Development")
SET(CPACK_COMPONENT_HEADERS_GROUP "Development")

# Expand the "Development" group by default, since we have so few components.
# Also, provide this group with a description.
SET(CPACK_COMPONENT_GROUP_DEVELOPMENT_EXPANDED ON)
SET(CPACK_COMPONENT_GROUP_DEVELOPMENT_DESCRIPTION
  "All of the tools you'll ever need to develop software")

# It doesn't make sense to install the headers without the libraries
# (because you could never use the headers!), so make the headers component
# depend on the libraries component.
SET(CPACK_COMPONENT_HEADERS_DEPENDS libraries)

# Create two installation types with pre-selected components.
# The "Developer" installation has just the library and headers,
# while the "Full" installation has everything.
SET(CPACK_ALL_INSTALL_TYPES Full Developer)
SET(CPACK_INSTALL_TYPE_FULL_DISPLAY_NAME "Everything")
SET(CPACK_COMPONENT_LIBRARIES_INSTALL_TYPES Developer Full)
SET(CPACK_COMPONENT_HEADERS_INSTALL_TYPES Developer Full)
#SET(CPACK_COMPONENT_APPLICATIONS_INSTALL_TYPES Full)

# Set license file to add
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License")

# Include CPack to introduce the appropriate targets
INCLUDE(CPack)
