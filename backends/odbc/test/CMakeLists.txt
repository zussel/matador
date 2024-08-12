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

set(ODBC_CONNECTION_STRING "mssql://test.db")

configure_file(Connection.hpp.in ${PROJECT_BINARY_DIR}/backends/odbc/test/connection.hpp @ONLY IMMEDIATE)

message(STATUS "odbc connection string: ${ODBC_CONNECTION_STRING}")

set(TEST_SOURCES
  ../../tests/QueryTest.cpp
  ../../tests/QueryTest.cpp
  ../../tests/ConnectionTest.cpp
  ../../tests/QueryRecordTest.cpp
  ../../tests/StatementTest.cpp
  ../../tests/TypeTraitsTest.cpp
  ../../tests/StatementCacheTest.cpp
  ../../tests/SessionTest.cpp
  ../../tests/ColorEnumTraits.hpp
  ../../tests/ColorEnumTraits.cpp
        ../../../test/catch2/models/types.hpp
        ../../tests/QueryBasicTest.cpp)

set(LIBRARY_TEST_TARGET odbc_tests)

add_executable(${LIBRARY_TEST_TARGET} ${TEST_SOURCES})

target_link_libraries(${LIBRARY_TEST_TARGET} PRIVATE
  Catch2::Catch2WithMain
  matador-utils
  matador-query
  ${CMAKE_DL_LIBS}
  ${ODBC_LIBRARIES})

target_include_directories(${LIBRARY_TEST_TARGET}
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/include
  PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/test/catch2
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

catch_discover_tests(${LIBRARY_TEST_TARGET} TEST_SUFFIX " (ODBC)")
