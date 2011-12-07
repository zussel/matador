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
  include/object/object_linker.hpp
  include/object/object_view.hpp
  include/object/object_proxy.hpp
  include/object/object_serializer.hpp
  include/object/prototype_node.hpp
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
  src/database/transaction.cpp
)

SET(DATABASE_HEADER
  include/database/action.hpp
  include/database/database.hpp
  include/database/transaction.hpp
)

ADD_LIBRARY(oos SHARED
  ${OBJECT_SOURCES} ${OBJECT_HEADER}
  ${TOOLS_SOURCES} ${TOOLS_HEADER}
  ${UNIT_SOURCES} ${UNIT_HEADER}
  ${DATABASE_SOURCES} ${DATABASE_HEADER}
)

ADD_SUBDIRECTORY(test)
