# Integration tests: need a running PostgreSQL server. Catch2 is provided by test/CMakeLists.txt.
set(MATADOR_POSTGRES_TEST_CONNECTION "$ENV{MATADOR_POSTGRES_TEST_CONNECTION}" CACHE STRING
    "Connection string used by the PostgreSQL integration tests")
set(POSTGRES_CONNECTION_STRING "${MATADOR_POSTGRES_TEST_CONNECTION}")

configure_file(Connection.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/connection.hpp @ONLY)

if(NOT POSTGRES_CONNECTION_STRING)
  message(WARNING "MATADOR_POSTGRES_TEST_CONNECTION is empty; PostgreSQL tests will not be able to connect")
endif()

set(TEST_SOURCES
  ../tests/ConnectionTest.cpp
  ../tests/QueryBasicTest.cpp
  ../tests/fixtures/QueryFixture.cpp

  # Backend-agnostic tests from ../ can be added here, e.g. ../ColorEnumTraits.cpp
)

add_executable(PostgresTests ${TEST_SOURCES})

target_link_libraries(PostgresTests PRIVATE
  matador
  Catch2::Catch2WithMain
  ${CMAKE_DL_LIBS})

# matador-postgres is a MODULE (loaded at runtime), so it can't be linked, only built first.
add_dependencies(PostgresTests matador-postgres)

target_include_directories(PostgresTests
  PRIVATE ${PROJECT_SOURCE_DIR}/test
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

#include(Catch)
#catch_discover_tests(PostgresTests TEST_SUFFIX " (PostgreSQL)" PROPERTIES LABELS postgres)
