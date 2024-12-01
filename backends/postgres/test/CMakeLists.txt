Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.5.4 # or a later release
)

FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)

set(POSTGRES_CONNECTION_STRING "postgres://test:test123@localhost:15432/testdb")

configure_file(Connection.hpp.in ${PROJECT_BINARY_DIR}/backends/postgres/test/connection.hpp @ONLY IMMEDIATE)

message(STATUS "postgresql connection string: ${POSTGRES_CONNECTION_STRING}")

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

set(LIBRARY_TEST_TARGET postgres_tests)

add_executable(${LIBRARY_TEST_TARGET} ${TEST_SOURCES})

target_link_libraries(${LIBRARY_TEST_TARGET} PRIVATE
  Catch2::Catch2WithMain
  matador-utils
  matador-query
  ${CMAKE_DL_LIBS}
  ${PostgreSQL_LIBRARY})

add_dependencies(${LIBRARY_TEST_TARGET} matador-postgres)

target_include_directories(${LIBRARY_TEST_TARGET}
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/include
  PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/test/catch2
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

catch_discover_tests(${LIBRARY_TEST_TARGET} TEST_SUFFIX " (PostgreSQL)")
