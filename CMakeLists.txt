# project name
PROJECT(OOS)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# project version
SET(${PROJECT_NAME}_MAJOR_VERSION 0)
SET(${PROJECT_NAME}_MINOR_VERSION 1)
SET(${PROJECT_NAME}_PATCH_LEVEL 0)

IF(CMAKE_COMPILER_IS_GNUCXX)
    MESSAGE(STATUS "GCC detected - Adding compiler flags")
    SET(CMAKE_CXX_FLAGS "-ggdb -Wextra -Wall -pedantic")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)

# Put the libaries and binaries that get built into directories at the
# top of the build tree rather than in hard-to-find leaf
# directories. This simplifies manual testing and the use of the build
# tree rather than installed Boost libraries.
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#IF (NOT CMAKE_BUILD_TYPE)
#  SET(CMAKE_BUILD_TYPE "Debug")
#ENDIF()

SET(OBJECT_SOURCES
  src/object/object.cpp
  src/object/object_atomizer.cpp
  src/object/object_creator.cpp
  src/object/object_deleter.cpp
  src/object/object_ptr.cpp
  src/object/object_store.cpp
  src/object/object_list.cpp
  src/object/object_vector.cpp
  src/object/linked_object_list.cpp
  src/object/object_linker.cpp
  src/object/object_proxy.cpp
  src/object/object_serializer.cpp
  src/object/prototype_node.cpp
)

SET(OBJECT_HEADER
  include/object/object.hpp
  include/object/object_atomizer.hpp
  include/object/object_creator.hpp
  include/object/object_deleter.hpp
  include/object/object_exception.hpp
  include/object/object_ptr.hpp
  include/object/object_store.hpp
  include/object/object_list.hpp
  include/object/object_vector.hpp
  include/object/linked_object_list.hpp
  include/object/object_linker.hpp
  include/object/object_value.hpp
  include/object/object_view.hpp
  include/object/object_proxy.hpp
  include/object/object_serializer.hpp
  include/object/prototype_node.hpp
  include/object/object_observer.hpp
)

SET(TOOLS_SOURCES
  src/tools/byte_buffer.cpp
)

SET(TOOLS_HEADER
  include/tools/byte_buffer.hpp
)

SET(UNIT_SOURCES
  src/unit/test_suite.cpp
  src/unit/unit_exception.cpp
  src/unit/unit_test.cpp
)

SET(UNIT_HEADER
  include/unit/test_suite.hpp
  include/unit/unit_exception.hpp
  include/unit/unit_test.hpp
)

SET(DATABASE_SOURCES
  src/database/action.cpp
  src/database/database.cpp
  src/database/inserter.cpp
  src/database/updater.cpp
  src/database/reader.cpp
  src/database/transaction.cpp
  src/database/result.cpp
  src/database/row.cpp
  src/database/statement.cpp
  src/database/statement_helper.cpp
)

SET(DATABASE_HEADER
  include/database/action.hpp
  include/database/database.hpp
  include/database/inserter.hpp
  include/database/updater.hpp
  include/database/reader.hpp
  include/database/transaction.hpp
  include/database/result.hpp
  include/database/row.hpp
  include/database/statement.hpp
  include/database/statement_helper.hpp
)

ADD_LIBRARY(oos SHARED
  ${OBJECT_SOURCES} ${OBJECT_HEADER}
  ${TOOLS_SOURCES} ${TOOLS_HEADER}
  ${UNIT_SOURCES} ${UNIT_HEADER}
  ${DATABASE_SOURCES} ${DATABASE_HEADER}
)

SET(SQLITE_DATABASE_SOURCES
  src/database/sqlite/sqlite_database.cpp
  src/database/sqlite/sqlite_statement.cpp
)

SET(SQLITE_DATABASE_HEADER
  include/database/sqlite/sqlite_database.hpp
  include/database/sqlite/sqlite_statement.hpp
)

ADD_LIBRARY(oos-sqlite SHARED
  ${SQLITE_DATABASE_SOURCES} ${SQLITE_DATABASE_HEADER}
)

TARGET_LINK_LIBRARIES(oos-sqlite oos)

# Group source files for IDE source explorers (e.g. Visual Studio)
SOURCE_GROUP("object" FILES ${OBJECT_SOURCES} ${OBJECT_HEADER})
SOURCE_GROUP("tools" FILES ${TOOLS_SOURCES} ${TOOLS_HEADER})
SOURCE_GROUP("unit" FILES ${UNIT_SOURCES} ${UNIT_HEADER})
SOURCE_GROUP("database" FILES ${DATABASE_SOURCES} ${DATABASE_HEADER})

ADD_SUBDIRECTORY(test)
