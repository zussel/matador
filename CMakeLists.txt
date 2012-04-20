# project name
PROJECT(OOS)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# project version
SET(${PROJECT_NAME}_MAJOR_VERSION 0)
SET(${PROJECT_NAME}_MINOR_VERSION 1)
SET(${PROJECT_NAME}_PATCH_LEVEL 0)

IF(CMAKE_COMPILER_IS_GNUCXX)
    MESSAGE(STATUS "GCC detected - Adding compiler flags")
    SET(CMAKE_CXX_FLAGS "-std=c++11 -std=gnu++11 -ggdb -Wextra -Wall -pedantic")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/include
	"C:/Programme/SQlite/include"
)

# add module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Put the libaries and binaries that get built into directories at the
# top of the build tree rather than in hard-to-find leaf
# directories. This simplifies manual testing and the use of the build
# tree rather than installed Boost libraries.
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


MESSAGE(STATUS "Looking for SQLite3")
FIND_PACKAGE(SQLite3 REQUIRED)
IF(SQLITE3_FOUND)
	MESSAGE(STATUS "Looking for SQLite3 - found")
ELSE(SQLITE3_FOUND)
	MESSAGE(FATAL_ERROR "Looking for SQLite3 - not found")
ENDIF(SQLITE3_FOUND)

#IF (NOT CMAKE_BUILD_TYPE)
#  SET(CMAKE_BUILD_TYPE "Debug")
#ENDIF()

SET(OBJECT_SOURCES
  src/object/object.cpp
  src/object/object_creator.cpp
  src/object/object_deleter.cpp
  src/object/object_ptr.cpp
  src/object/object_store.cpp
  src/object/object_value.cpp
  src/object/object_proxy.cpp
  src/object/object_serializer.cpp
  src/object/prototype_node.cpp
)

SET(OBJECT_HEADER
  include/object/object.hpp
  include/object/object_atomizer.hpp
  include/object/object_atomizable.hpp
  include/object/object_creator.hpp
  include/object/object_deleter.hpp
  include/object/object_exception.hpp
  include/object/object_ptr.hpp
  include/object/object_store.hpp
  include/object/object_container.hpp
  include/object/object_list.hpp
  include/object/object_vector.hpp
  include/object/linked_object_list.hpp
  include/object/object_value.hpp
  include/object/object_view.hpp
  include/object/object_proxy.hpp
  include/object/object_serializer.hpp
  include/object/prototype_node.hpp
  include/object/object_observer.hpp
  include/object/object_expression.hpp
)

SET(TOOLS_SOURCES
  src/tools/byte_buffer.cpp
  src/tools/library.cpp
  src/tools/blob.cpp
  src/tools/varchar.cpp
  src/tools/sequencer.cpp
)

SET(TOOLS_HEADER
  include/tools/algorithm.hpp
  include/tools/byte_buffer.hpp
  include/tools/singleton.hpp
  include/tools/library.hpp
  include/tools/blob.hpp
  include/tools/varchar.hpp
  include/tools/sequencer.hpp
  include/tools/factory.hpp
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
  src/database/session.cpp
  src/database/database.cpp
  src/database/database_exception.cpp
  src/database/database_factory.cpp
  src/database/transaction.cpp
  src/database/transaction_helper.cpp
  src/database/result.cpp
  src/database/row.cpp
  src/database/statement.cpp
  src/database/statement_creator.cpp
  src/database/statement_serializer.cpp
)

SET(DATABASE_HEADER
  include/database/action.hpp
  include/database/session.hpp
  include/database/database.hpp
  include/database/database_exception.hpp
  include/database/database_factory.hpp
  include/database/database_sequencer.hpp
  include/database/transaction.hpp
  include/database/transaction_helper.hpp
  include/database/result.hpp
  include/database/row.hpp
  include/database/value.hpp
  include/database/statement.hpp
  include/database/statement_creator.hpp
  include/database/statement_serializer.hpp
)

ADD_LIBRARY(oos SHARED
  ${OBJECT_SOURCES} ${OBJECT_HEADER}
  ${TOOLS_SOURCES} ${TOOLS_HEADER}
  ${UNIT_SOURCES} ${UNIT_HEADER}
  ${DATABASE_SOURCES} ${DATABASE_HEADER}
)

SET(SQLITE_DATABASE_SOURCES
  src/database/sqlite/sqlite_database.cpp
  src/database/sqlite/sqlite_exception.cpp
  src/database/sqlite/sqlite_statement.cpp
  src/database/sqlite/sqlite_sequencer.cpp
  src/database/sqlite/sqlite_result.cpp
  src/database/sqlite/sqlite_producer.cpp
)

SET(SQLITE_DATABASE_HEADER
  include/database/sqlite/sqlite_database.hpp
  include/database/sqlite/sqlite_exception.hpp
  include/database/sqlite/sqlite_statement.hpp
  include/database/sqlite/sqlite_sequencer.hpp
  include/database/sqlite/sqlite_result.hpp
  include/database/sqlite/sqlite_producer.hpp
  include/database/sqlite/sqlite_types.hpp
)

ADD_LIBRARY(oos-sqlite SHARED
  ${SQLITE_DATABASE_SOURCES} ${SQLITE_DATABASE_HEADER}
)

TARGET_LINK_LIBRARIES(oos-sqlite oos ${SQLITE3_LIBRARY})

# Group source files for IDE source explorers (e.g. Visual Studio)
SOURCE_GROUP("object" FILES ${OBJECT_SOURCES} ${OBJECT_HEADER})
SOURCE_GROUP("tools" FILES ${TOOLS_SOURCES} ${TOOLS_HEADER})
SOURCE_GROUP("unit" FILES ${UNIT_SOURCES} ${UNIT_HEADER})
SOURCE_GROUP("database" FILES ${DATABASE_SOURCES} ${DATABASE_HEADER})

ADD_SUBDIRECTORY(test)
ADD_SUBDIRECTORY(example)
