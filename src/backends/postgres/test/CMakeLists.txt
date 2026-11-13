CPMAddPackage("gh:catchorg/Catch2@3.14.0")

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

set(POSTGRES_CONNECTION_STRING "postgres://test:test123!@127.0.0.1:15442/matador")
#set(POSTGRES_CONNECTION_STRING "postgres://test:test123!@127.0.0.1:5432/matador")

configure_file(Connection.hpp.in ${PROJECT_BINARY_DIR}/backends/postgres/test/connection.hpp @ONLY IMMEDIATE)

message(STATUS "postgresql connection string: ${POSTGRES_CONNECTION_STRING}")

set(TEST_SOURCES
  ../../../test/backends/ColorEnumTraits.cpp
  ../../../test/backends/ColorEnumTraits.hpp
  ../../../test/backends/ConnectionTest.cpp
  ../../../test/backends/QueryBasicTest.cpp
  ../../../test/backends/QueryFixture.cpp
  ../../../test/backends/QueryFixture.hpp
  ../../../test/backends/QueryRecordTest.cpp
  ../../../test/backends/QueryStatementTests.cpp
  ../../../test/backends/QueryTest.cpp
  ../../../test/backends/SchemaFixture.cpp
  ../../../test/backends/SchemaFixture.hpp
  ../../../test/backends/SchemaTest.cpp
  ../../../test/backends/SequenceFixture.cpp
  ../../../test/backends/SequenceFixture.hpp
  ../../../test/backends/SequenceTest.cpp
  ../../../test/backends/SessionDeleteHasMany.cpp
  ../../../test/backends/SessionDeleteHasManyToMany.cpp
  ../../../test/backends/SessionFixture.cpp
  ../../../test/backends/SessionFixture.hpp
  ../../../test/backends/SessionInsertBelongsTo.cpp
  ../../../test/backends/SessionInsertHasMany.cpp
  ../../../test/backends/SessionInsertHasManyToManyTest.cpp
  ../../../test/backends/SessionInsertHasOne.cpp
  ../../../test/backends/SessionTest.cpp
  ../../../test/backends/StatementCacheTest.cpp
  ../../../test/backends/StatementTest.cpp
  ../../../test/backends/TableSequenceFixture.cpp
  ../../../test/backends/TableSequenceFixture.hpp
  ../../../test/backends/TableSequenceTest.cpp
  ../../../test/backends/TypeTraitsTest.cpp
  ../../../test/models/author.hpp
  ../../../test/models/coordinate.hpp
  ../../../test/models/location.hpp
  ../../../test/models/model_metas.hpp
  ../../../test/models/types.hpp
  ../../../test/models/user.hpp
  ../../../test/utils/RecordPrinter.cpp
  ../../../test/utils/RecordPrinter.hpp
)

set(LIBRARY_TEST_TARGET PostgresTests)

add_executable(${LIBRARY_TEST_TARGET} ${TEST_SOURCES})

target_link_libraries(${LIBRARY_TEST_TARGET} PRIVATE
  Catch2::Catch2WithMain
  matador-core
  matador-orm
  ${CMAKE_DL_LIBS}
  ${PostgreSQL_LIBRARY})

add_dependencies(${LIBRARY_TEST_TARGET} matador-postgres)

target_include_directories(${LIBRARY_TEST_TARGET}
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/include
  PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/test
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

#catch_discover_tests(${LIBRARY_TEST_TARGET} TEST_SUFFIX " (PostgreSQL)")
