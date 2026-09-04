cmake_minimum_required(VERSION 3.25)

# project name
project(
        matador
        VERSION 0.9.2
        DESCRIPTION "A sql library"
        LANGUAGES C CXX
)


# project version
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)

message(STATUS "${PROJECT_NAME_UPPER} ${APP_VERSION}")

# Common compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# These are defined for clang/gcc compatibility.
# When non-compatible flags are implemented then we must specify them separately.
SET(GCC_CLANG_COMMON_FLAGS "-Wall -Wextra -pedantic -ftemplate-backtrace-limit=0")
SET(GCC_CLANG_COMMON_FLAGS_DEBUG "-O0 -g -DDEBUG")
SET(GCC_CLANG_COMMON_FLAGS_RELEASE "-O1 -DNDEBUG")
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
SET(CMAKE_BUILD_PARALLEL_LEVEL 4)

#if(CMAKE_COMPILER_IS_GNUCXX)
#    message(STATUS "GCC detected - Adding compiler flags")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_CLANG_COMMON_FLAGS}")
#    set(CMAKE_CXX_FLAGS_DEBUG "${GCC_CLANG_COMMON_FLAGS_DEBUG}")
#    set(CMAKE_CXX_FLAGS_RELEASE "${GCC_CLANG_COMMON_FLAGS_RELEASE}")
#elseif(CMAKE_CXX_COMPILER MATCHES "clang")
#    message(STATUS "Clang detected - Adding compiler flags")
#    set(CMAKE_CXX_FLAGS "${GCC_CLANG_COMMON_FLAGS}")
#    set(CMAKE_CXX_FLAGS_DEBUG "${GCC_CLANG_COMMON_FLAGS_DEBUG}")
#    set(CMAKE_CXX_FLAGS_RELEASE "${GCC_CLANG_COMMON_FLAGS_RELEASE}")
#elseif(MSVC)
#    message(STATUS "MSVC detected - Adding compiler flags")
#    set(CMAKE_CXX_FLAGS "/W3 /EHsc /bigobj")
#    set(CMAKE_CXX_FLAGS_DEBUG "/MDd /Od /Zi /D_DEBUG /DDEBUG")
#    set(CMAKE_CXX_FLAGS_RELEASE "/O1 /DNDEBUG")
#endif()

option(ENABLE_CLANG_TIDY "Enable clang-tidy static analysis" ON)

message(STATUS "Architecture: ${CMAKE_SYSTEM_PROCESSOR} ${CMAKE_SYSTEM_NAME}")

set(SYSTEM_NAME_LOWER)
string(TOLOWER ${CMAKE_SYSTEM_NAME} SYSTEM_NAME_LOWER)

# add module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

if (NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE "Release")
else()
  message(STATUS "Current build type ${CMAKE_BUILD_TYPE}")
endif()

#OPTION(COVERAGE "Enable generation of code coverage" false)
#IF (NOT COVERAGE_TESTS)
#  SET(COVERAGE_TESTS "all")
#ENDIF()

#OPTION(ARCH "Compiler architecture for Clang/GCC" "")
OPTION(EXAMPLES "Build examples" true)

FIND_PACKAGE( Threads REQUIRED )

#if (NOT MSVC AND (COVERAGE) AND CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT CMAKE_CXX_COMPILER MATCHES "clang")
#  MESSAGE(STATUS "coverage for compiler ${CMAKE_CXX_COMPILER}")
#  MESSAGE(STATUS "coverage tests are: ${COVERAGE_TESTS}")
#
#  SET(GCOV_BINARY "gcov" CACHE STRING "gcov binary")
#
#  MESSAGE(STATUS "gcov binary: ${GCOV_BINARY}")
#
##  INCLUDE(CodeCoverage)
#  MESSAGE(STATUS "will generate coverage data")
#  SET(GCOV_ADDITIONAL_FLAGS "-fprofile-arcs -ftest-coverage")
#  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCOV_ADDITIONAL_FLAGS}")
#  SET(CMAKE_C_FLAGS "{CMAKE_C_FLAGS} ${GCOV_ADDITIONAL_FLAGS}")
#  SET(CMAKE_EXE_LINKER_FLAGS "${GCOV_ADDITIONAL_FLAGS} ")
#
##  SETUP_TARGET_FOR_COVERAGE(coverage test_matador src coverage "exec;${COVERAGE_TESTS}")
#else()
#  MESSAGE(STATUS "no coverage for compiler ${CMAKE_CXX_COMPILER}")
#endif()

SET(NET_LIBRARIES)

#if (WIN32)
#  find_library(WSOCK32_LIBRARY wsock32)
#  find_library(WS2_32_LIBRARY ws2_32)

#  message(STATUS "WSOCK32 ${WSOCK32_LIBRARY}")
#  message(STATUS "WS2_32 ${WS2_32_LIBRARY}")

#  MESSAGE(STATUS "Appending Windows Socket libs")
#  LIST(APPEND NET_LIBRARIES wsock32 ws2_32)
#  LIST(APPEND NET_LIBRARIES ${WSOCK32_LIBRARY} ${WS2_32_LIBRARY})
#endif()
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
include_directories(${PROJECT_SOURCE_DIR}/include)

#SET(BACKENDS
#  SQLite3
#  MySQL
#  ODBC
#  PostgreSQL
#)
#
#FOREACH(backend ${BACKENDS})
#  STRING(TOUPPER ${backend} BACKEND_UPPER)
#  OPTION(${PROJECT_NAME_UPPER}_${BACKEND_UPPER} "Enable or disable backend ${backend}" TRUE)
#  OPTION(${PROJECT_NAME_UPPER}_${BACKEND_UPPER}_TEST "Enable or disable backend ${backend} tests" TRUE)
#
#  IF (${PROJECT_NAME_UPPER}_${BACKEND_UPPER})
#    FIND_PACKAGE(${backend})
#    IF (${BACKEND_UPPER}_FOUND)
#      ADD_DEFINITIONS(-D${PROJECT_NAME_UPPER}_${BACKEND_UPPER})
#      IF (${PROJECT_NAME_UPPER}_${BACKEND_UPPER}_TEST)
#        MESSAGE(STATUS "Enable ${backend} tests")
#        ADD_DEFINITIONS(-D${PROJECT_NAME_UPPER}_${BACKEND_UPPER}_TEST)
#      ENDIF()
#    ELSE()
#      MESSAGE(STATUS "No backend ${backend}")
#    ENDIF()
#  ELSE()
#    MESSAGE(STATUS "Backend ${backend} disabled")
#  ENDIF()
#ENDFOREACH(backend)
#
#INCLUDE(Matador)

include_directories(${PROJECT_BINARY_DIR})

# https://cmake.org/Wiki/CMake_RPATH_handling
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
SET(CMAKE_MACOSX_RPATH TRUE)

# contains the full path to the top level directory of your build tree
message( STATUS "PROJECT_BINARY_DIR: " ${PROJECT_BINARY_DIR} )

# contains the full path to the root of your project source directory,
# i.e. to the nearest directory where CMakeLists.txt contains the PROJECT() command
message( STATUS "PROJECT_SOURCE_DIR: " ${PROJECT_SOURCE_DIR} )

message( STATUS "CMAKE_BINARY_DIR: " ${CMAKE_BINARY_DIR} )

include(cmake/CPM.cmake)

# Clang-Tidy Configuration
if(ENABLE_CLANG_TIDY)
  find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
  if(CLANG_TIDY_EXE)
    message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
  else()
    message(WARNING "ENABLE_CLANG_TIDY is ON, but clang-tidy executable was not found.")
  endif()
endif()

include(CTest)

add_subdirectory(src)
add_subdirectory(doc)
add_subdirectory(test)
if (EXAMPLES)
  add_subdirectory(examples EXCLUDE_FROM_ALL)
#  ADD_SUBDIRECTORY(sandbox EXCLUDE_FROM_ALL)
endif()

#INSTALL(
#  DIRECTORY ${PROJECT_BINARY_DIR}/doc/web/
#  DESTINATION share/doc/matador
#  COMPONENT doc
#)
#
#set(ARCHPKG_ARCH ${CMAKE_SYSTEM_PROCESSOR})
#if (${ARCH} MATCHES "32")
#  set(ARCHPKG_ARCH "x86")
#elseif(${ARCH} MATCHES "64")
#  set(CPACK_SYSTEM_NAME "x86_64")
#else()
#  set(ARCHPKG_ARCH ${CMAKE_SYSTEM_PROCESSOR})
#endif()
#
## Basic CPack configuration
#SET(CPACK_PACKAGE_NAME ${PROJECT_NAME})
#SET(CPACK_PACKAGE_VENDOR "zussel")
#SET(CPACK_PACKAGE_CONTACT "skuehl1972@gmail.com")
#SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "matador - take your database by the horns")
#SET(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/License")
#SET(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.md")
#SET(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME})
#
#if (CMAKE_SYSTEM_NAME MATCHES "Linux")
#  if (${ARCH} MATCHES "32")
#    SET(CPACK_SYSTEM_NAME "x86-${SYSTEM_NAME_LOWER}")
#  elseif(${ARCH} MATCHES "64")
#    SET(CPACK_SYSTEM_NAME "x86_64-${SYSTEM_NAME_LOWER}")
#  else()
#    SET(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_PROCESSOR}-${SYSTEM_NAME_LOWER}")
#  endif()
#endif()
#
#SET(CPACK_GENERATOR "DEB;RPM;STGZ;TGZ;TZ")
#
## Tell CPack all of the components to install. The "ALL"
## refers to the fact that this is the set of components that
## will be included when CPack is instructed to put everything
## into the binary installer (the default behavior).
#SET(CPACK_COMPONENTS_ALL libraries headers doc)
#
## Set the displayed names for each of the components to install.
## These will be displayed in the list of components inside the installer.
#SET(CPACK_COMPONENT_APPLICATIONS_DISPLAY_NAME ${PROJECT_NAME})
#SET(CPACK_COMPONENT_LIBRARIES_DISPLAY_NAME "Libraries")
#SET(CPACK_COMPONENT_HEADERS_DISPLAY_NAME "${PROJECT_NAME} Headers")
#
## Provide descriptions for each of the components to install.
## When the user hovers the mouse over the name of a component,
## the description will be shown in the "Description" box in the
## installer. If no descriptions are provided, the "Description"
## box will be removed.
#SET(CPACK_COMPONENT_LIBRARIES_DESCRIPTION "Take your application by the horns")
#SET(CPACK_COMPONENT_HEADERS_DESCRIPTION "C/C++ header files for ${PROJECT_NAME}")
#
## Put the components into two different groups: "Runtime" and "Development"
##SET(CPACK_COMPONENT_APPLICATIONS_GROUP "Runtime")
#SET(CPACK_COMPONENT_LIBRARIES_GROUP "Development")
#SET(CPACK_COMPONENT_HEADERS_GROUP "Development")
#
## Expand the "Development" group by default, since we have so few components.
## Also, provide this group with a description.
#SET(CPACK_COMPONENT_GROUP_DEVELOPMENT_EXPANDED ON)
#SET(CPACK_COMPONENT_GROUP_DEVELOPMENT_DESCRIPTION "All of the tools you'll ever need to develop software")
#
## It doesn't make sense to install the headers without the libraries
## (because you could never use the headers!), so make the headers component
## depend on the libraries component.
#SET(CPACK_COMPONENT_HEADERS_DEPENDS libraries)
#
## Create two installation types with pre-selected components.
## The "Developer" installation has just the library and headers,
## while the "Full" installation has everything.
#SET(CPACK_ALL_INSTALL_TYPES Full Developer)
#SET(CPACK_INSTALL_TYPE_FULL_DISPLAY_NAME "Everything")
#SET(CPACK_COMPONENT_LIBRARIES_INSTALL_TYPES Developer Full)
#SET(CPACK_COMPONENT_HEADERS_INSTALL_TYPES Developer Full)
##SET(CPACK_COMPONENT_APPLICATIONS_INSTALL_TYPES Full)
#
#SET(CPACK_PACKAGE_DIRECTORY "${PROJECT_SOURCE_DIR}/Package")
#
## Debian package settings
#set(CPACK_DEB_COMPONENT_INSTALL OFF)
#set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
#set(CPACK_DEBIAN_COMPRESSION_TYPE "gzip")
#set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
#set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://zussel.github.io/matador")
#set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS OFF)
#set(CPACK_DEBIAN_PACKAGE_CONTROL_STRICT_PERMISSION ON)
##set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_SOURCE_DIR}/packaging/postinst;")
#set(CPACK_DEBIAN_PACKAGE_NAME "${PROJECT_NAME}")
#set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6")
#
## RPM package settings
#set(CPACK_RPM_COMPONENT_INSTALL OFF)
#set(CPACK_RPM_PACKAGE_RELEASE "1")
#set(CPACK_RPM_PACKAGE_SUMMARY "Matador")
#set(CPACK_RPM_PACKAGE_DESCRIPTION "Matador - a c++ orm library")
#set(CPACK_RPM_PACKAGE_LICENSE "GPLv3")
#set(CPACK_RPM_PACKAGE_VENDOR "zussel")
#set(CPACK_RPM_PACKAGE_URL "https://zussel.github.io/matador")
#set(CPACK_RPM_PACKAGE_AUTOREQ 0)
#set(CPACK_RPM_PACKAGE_AUTOPROV 0)
#
## Include CPack to introduce the appropriate targets
#INCLUDE(CPack)
