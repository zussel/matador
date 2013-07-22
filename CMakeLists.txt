# project name
PROJECT(oos)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# project version
SET(${PROJECT_NAME}_MAJOR_VERSION 0)
SET(${PROJECT_NAME}_MINOR_VERSION 1)
SET(${PROJECT_NAME}_PATCH_LEVEL 0)
SET(${PROJECT_NAME}_VERSION 0.1.0)

IF(CMAKE_COMPILER_IS_GNUCXX)
    MESSAGE(STATUS "GCC detected - Adding compiler flags")
    
    EXECUTE_PROCESS(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    if (GCC_VERSION VERSION_GREATER 4.6)
        MESSAGE(STATUS "Version > 4.6")
        SET(CMAKE_CXX_FLAGS "-std=c++11 -std=gnu++11 -Wextra -Wall -pedantic")
    else ()
        MESSAGE(STATUS "Version <= 4.6")
        SET(CMAKE_CXX_FLAGS "-std=c++0x -std=gnu++0x -Wextra -Wall -pedantic")
    endif()
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

MESSAGE(STATUS "Architecture: ${CMAKE_SYSTEM_PROCESSOR}")
MESSAGE(STATUS "Architecture OSX: ${CMAKE_OSX_ARCHITECTURES}")
MESSAGE(STATUS "Architecture Size: ${CMAKE_SIZEOF_VOID_P}")
MESSAGE(STATUS "Architecture System: ${CMAKE_SYSTEM_NAME}")

SET(OS)

STRING(TOLOWER ${CMAKE_SYSTEM_NAME} OS)

MESSAGE(STATUS "Architecture System: ${OS}")
#MESSAGE(STATUS "Version: ${{PROJECT_NAME}_VERSION}")
#MESSAGE(STATUS "Package name: ${PROJECT_NAME}-${{PROJECT_NAME}_VERSION}")

FIND_PACKAGE(Doxygen)

if(DOXYGEN_FOUND)
    MESSAGE(STATUS "Doxygen detected")
    # Configure the Template Doxyfile for our specific project
    configure_file(doc/doxygen/oos-api.dxy.in ${PROJECT_BINARY_DIR}/oos-api.dxy @ONLY IMMEDIATE)
    configure_file(doc/doxygen/oos.dxy.in ${PROJECT_BINARY_DIR}/oos.dxy @ONLY IMMEDIATE)
    # Add a custom target to run Doxygen when ever the project is built
    #add_custom_target (doc ALL 
    add_custom_target (doc
  										 COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/oos-api.dxy
  										 COMMENT "Generating API documentation with Doxygen" VERBATIM)
    add_custom_target (web
  										 COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/oos.dxy
                       COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/doc/web ${CMAKE_BINARY_DIR}/web
  										 COMMENT "Generating WEB documentation with Doxygen" VERBATIM)
ENDIF(DOXYGEN_FOUND)

# add module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

IF (NOT CMAKE_BUILD_TYPE)
  MESSAGE(STATUS "No build type selected, default to Release")
  SET(CMAKE_BUILD_TYPE "Release")
ENDIF()

# Put the libaries and binaries that get built into directories at the
# top of the build tree rather than in hard-to-find leaf
# directories. This simplifies manual testing and the use of the build
# tree rather than installed Boost libraries.
#IF (WIN32)
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib)
  SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib)
  SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin)
  SET(CMAKE_MODULE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin)
#ENDIF (WIN32)

#FIND_PACKAGE(EXPAT REQUIRED PATHS "C:/Programme/Expat/Bin" "C:/Programme/Expat/Source/lib")

#SET(Boost_ADDITIONAL_VERSIONS "1.49" "1.49.0")
#FIND_PACKAGE(Boost 1.49.0 REQUIRED)

#IF (NOT Boost_FOUND)
#	MESSAGE(FATAL_ERROR "Couldn't find boost")
#ENDIF()

MESSAGE(STATUS "Looking for SQLite3")
FIND_PACKAGE(SQLite3)
IF(SQLITE3_FOUND)
	MESSAGE(STATUS "Found SQLite3")
ELSE(SQLITE3_FOUND)
  MESSAGE(STATUS "SQLite not found - not building backend")
ENDIF(SQLITE3_FOUND)

MESSAGE(STATUS "Looking for MySQL")
FIND_PACKAGE(MySQL)
IF(MYSQL_FOUND)
	MESSAGE(STATUS "Found MySQL")
ELSE(MYSQL_FOUND)
  MESSAGE(STATUS "MySQL not found - not building backend")
ENDIF(MYSQL_FOUND)

MESSAGE(STATUS "SQLite3 include: ${SQLITE3_INCLUDE_DIR}")

#MESSAGE(STATUS "MySQL lib: ${MYSQL_LIBRARY}")

#
# configure include dir
#

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)

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

CONFIGURE_FILE (${PROJECT_SOURCE_DIR}/include/version.hpp.in ${PROJECT_BINARY_DIR}/version.hpp)

GET_PROPERTY(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
FOREACH(dir ${dirs})
  MESSAGE(STATUS "include dir='${dir}'")
ENDFOREACH()


# contains the full path to the top level directory of your build tree 
MESSAGE( STATUS "PROJECT_BINARY_DIR: " ${PROJECT_BINARY_DIR} )

# contains the full path to the root of your project source directory,
# i.e. to the nearest directory where CMakeLists.txt contains the PROJECT() command 
MESSAGE( STATUS "PROJECT_SOURCE_DIR: " ${PROJECT_SOURCE_DIR} )

MESSAGE( STATUS "CMAKE_BINARY_DIR: " ${CMAKE_BINARY_DIR} )

ENABLE_TESTING()
ADD_SUBDIRECTORY(src)
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
SET(CPACK_PACKAGE_VENDOR "Elephant Sky")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "OOS Open Object Store - Store all your object in one container")
SET(CPACK_PACKAGE_VERSION "0.1.0")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "1")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "oos")

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
