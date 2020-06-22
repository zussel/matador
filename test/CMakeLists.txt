SET (TEST_TOOLS_SOURCES
  utils/TimeTestUnit.cpp
  utils/TimeTestUnit.hpp
  utils/DateTestUnit.cpp
  utils/DateTestUnit.hpp
  utils/BlobTestUnit.hpp
  utils/BlobTestUnit.cpp
  utils/FactoryTestUnit.hpp
  utils/FactoryTestUnit.cpp
  utils/StringTestUnit.cpp
  utils/StringTestUnit.hpp
  utils/AnyTestUnit.cpp
  utils/AnyTestUnit.hpp
  utils/SequencerTestUnit.cpp
  utils/SequencerTestUnit.hpp
  utils/JsonTestUnit.cpp
  utils/JsonTestUnit.hpp
  utils/JsonMapperTestUnit.cpp
  utils/JsonMapperTestUnit.hpp
  utils/OSTest.cpp
  utils/OSTest.hpp
  utils/FileTestUnit.cpp utils/FileTestUnit.hpp)

SET (TEST_LOGGER_SOURCES
  logger/LoggerTest.cpp
  logger/LoggerTest.hpp
    logger/file.cpp logger/file.hpp)

SET (TEST_HEADER datatypes.hpp entities.hpp person.hpp has_many_list.hpp Blog.hpp)

SET (TEST_OBJECT_SOURCES
  object/ObjectPrototypeTestUnit.cpp
  object/ObjectPrototypeTestUnit.hpp
  object/ObjectStoreTestUnit.cpp
  object/ObjectStoreTestUnit.hpp
  object/PrototypeTreeTest.cpp
  object/PrototypeTreeTest.hpp
  object/PrimaryKeyUnitTest.cpp
  object/PrimaryKeyUnitTest.hpp
  object/HasManyVectorUnitTest.cpp
  object/HasManyVectorUnitTest.hpp
  object/HasManyListUnitTest.cpp
  object/HasManyListUnitTest.hpp
  object/ObjectTransactionTestUnit.cpp
  object/ObjectTransactionTestUnit.hpp
  object/RelationTestUnit.cpp
  object/RelationTestUnit.hpp
  object/HasManyUnitTest.cpp
  object/HasManyUnitTest.hpp
  object/JsonObjectMapperTest.cpp
  object/JsonObjectMapperTest.hpp)

SET (TEST_UNIT_SOURCES
  unit/TestSuiteTestUnit.cpp
  unit/TestSuiteTestUnit.hpp
)

SET (TEST_SQL_SOURCES
  sql/ConnectionTestUnit.cpp
  sql/ConnectionTestUnit.hpp
  sql/QueryTestUnit.cpp
  sql/QueryTestUnit.hpp
  sql/ConditionUnitTest.cpp
  sql/ConditionUnitTest.hpp
  sql/DialectTestUnit.cpp
  sql/DialectTestUnit.hpp
  sql/TestDialect.cpp
  sql/TestDialect.hpp
  sql/MSSQLDialectTestUnit.cpp
  sql/MSSQLDialectTestUnit.hpp
  sql/SQLiteDialectTestUnit.cpp
  sql/SQLiteDialectTestUnit.hpp
  sql/ValueUnitTest.cpp
  sql/ValueUnitTest.hpp
  sql/PostgreSQLDialectTestUnit.cpp
  sql/PostgreSQLDialectTestUnit.hpp sql/SqlLoggerTest.cpp sql/SqlLoggerTest.hpp)

SET (TEST_ORM_SOURCES
  orm/TransactionTestUnit.cpp
  orm/TransactionTestUnit.hpp
  orm/OrmTestUnit.cpp
  orm/OrmTestUnit.hpp
  orm/OrmReloadTestUnit.cpp
  orm/OrmReloadTestUnit.hpp
  orm/OrmRelationTestUnit.cpp
  orm/OrmRelationTestUnit.hpp
  orm/BlogUnitTest.cpp
  orm/BlogUnitTest.hpp
  orm/PrimaryKeyTestUnit.cpp
  orm/PrimaryKeyTestUnit.hpp
)

SET (TEST_SOURCES test_matador.cpp)

ADD_EXECUTABLE(test_matador
  ${TEST_SOURCES}
  ${TEST_HEADER}
  ${TEST_TOOLS_SOURCES}
  ${TEST_OBJECT_SOURCES}
  ${TEST_UNIT_SOURCES}
  ${TEST_ORM_SOURCES}
  ${TEST_LOGGER_SOURCES}
  ${TEST_SQL_SOURCES}
)

SET(MYSQL_CONNECTION_STRING "mysql://sascha:sascha@127.0.0.1/matador_test" CACHE STRING "mysql connection string")
IF (WIN32)
  SET(MSSQL_CONNECTION_STRING "mssql://sascha:sascha@(local)\\\\SQLEXPRESS/test_oos (SQL Server)" CACHE STRING "mssql connection string")
ELSE()
  SET(MSSQL_CONNECTION_STRING "mssql://sascha:sascha@192.168.27.58/test (FreeTDS)" CACHE STRING "mssql connection string")
#  SET(MSSQL_CONNECTION_STRING "mssql://sascha:sascha@192.168.178.31\\\\SQLEXPRESS/test_matador (FreeTDS)" CACHE STRING "mssql connection string")
ENDIF()
SET(SQLITE_CONNECTION_STRING "sqlite://test.sqlite" CACHE STRING "sqlite connection string")
SET(POSTGRESQL_CONNECTION_STRING "postgresql://test:test123@127.0.0.1/matador_test" CACHE STRING "postgresql connection string")


MESSAGE(STATUS "mysql connection string: ${MYSQL_CONNECTION_STRING}")
MESSAGE(STATUS "mssql connection string: ${MSSQL_CONNECTION_STRING}")
MESSAGE(STATUS "sqlite connection string: ${SQLITE_CONNECTION_STRING}")
MESSAGE(STATUS "postgresql connection string: ${POSTGRESQL_CONNECTION_STRING}")

CONFIGURE_FILE(connections.hpp.in ${PROJECT_BINARY_DIR}/connections.hpp @ONLY IMMEDIATE)

SET(DB_LIBRARIES)

IF(ODBC_FOUND)
  MESSAGE(STATUS "Appendig ODBC libs: ${ODBC_LIBRARY}")
  LIST(APPEND DB_LIBRARIES ${ODBC_LIBRARY})
ENDIF()

IF(MYSQL_FOUND)
  MESSAGE(STATUS "Appendig MySQL libs: ${MYSQL_LIBRARY}")
  LIST(APPEND DB_LIBRARIES ${MYSQL_LIBRARY})
ENDIF()

IF(SQLITE3_FOUND)
  MESSAGE(STATUS "Appendig SQLite3 libs: ${SQLITE3_LIBRARY}")
  LIST(APPEND DB_LIBRARIES ${SQLITE3_LIBRARY})
ENDIF()

IF(POSTGRESQL_FOUND)
  MESSAGE(STATUS "Appendig PostgreSQL libs: ${POSTGRESQL_LIBRARIES}")
  LIST(APPEND DB_LIBRARIES ${POSTGRESQL_LIBRARIES})
ENDIF()

TARGET_LINK_LIBRARIES(test_matador
  matador-unit
  matador-utils
  matador-logger
  matador-object
  matador-sql
  matador-orm
  ${CMAKE_DL_LIBS}
  ${DB_LIBRARIES}
)

# Group source files for IDE source explorers (e.g. Visual Studio)
SOURCE_GROUP("object" FILES ${TEST_OBJECT_SOURCES})
SOURCE_GROUP("utils" FILES ${TEST_TOOLS_SOURCES})
SOURCE_GROUP("unit" FILES ${TEST_UNIT_SOURCES})
SOURCE_GROUP("orm" FILES ${TEST_ORM_SOURCES})
SOURCE_GROUP("sql" FILES ${TEST_SQL_SOURCES})
SOURCE_GROUP("main" FILES ${TEST_SOURCES})

MESSAGE(STATUS "Current binary dir: ${CMAKE_CURRENT_BINARY_DIR}")

ADD_TEST(test_matador_all ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_matador exec all)
