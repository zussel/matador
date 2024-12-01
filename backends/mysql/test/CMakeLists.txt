Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.4 # or a later release
)

FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)

set(MYSQL_CONNECTION_STRING "mysql://test:test123!@127.0.0.1:3306/matador_test")

configure_file(Connection.hpp.in ${PROJECT_BINARY_DIR}/backends/mysql/test/connection.hpp @ONLY IMMEDIATE)

message(STATUS "mysql connection string: ${MYSQL_CONNECTION_STRING}")

set(TEST_SOURCES
  ../../../test/catch2/models/types.hpp
  ../../tests/ColorEnumTraits.cpp
  ../../tests/ColorEnumTraits.hpp
  ../../tests/ConnectionTest.cpp
  ../../tests/QueryBasicTest.cpp
  ../../tests/QueryFixture.cpp
  ../../tests/QueryFixture.hpp
  ../../tests/QueryRecordTest.cpp
  ../../tests/QueryStatementTests.cpp
  ../../tests/QueryTest.cpp
  ../../tests/SessionFixture.cpp
  ../../tests/SessionFixture.hpp
  ../../tests/SessionTest.cpp
  ../../tests/StatementCacheTest.cpp
  ../../tests/StatementTest.cpp
  ../../tests/TypeTraitsTest.cpp
)

set(LIBRARY_TEST_TARGET mysql_tests)

add_executable(${LIBRARY_TEST_TARGET} ${TEST_SOURCES})

target_link_libraries(${LIBRARY_TEST_TARGET} PRIVATE
  Catch2::Catch2WithMain
  matador-utils
  matador-query
  ${CMAKE_DL_LIBS}
  ${MySQL_LIBRARY})

add_dependencies(${LIBRARY_TEST_TARGET} matador-mysql)

target_include_directories(${LIBRARY_TEST_TARGET}
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/include
  PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/test/catch2
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

catch_discover_tests(${LIBRARY_TEST_TARGET} TEST_SUFFIX " (MySQL)")
