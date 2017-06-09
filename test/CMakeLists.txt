SET (TEST_TOOLS_SOURCES
  utils/TimeTestUnit.cpp
  utils/TimeTestUnit.hpp
  utils/DateTestUnit.cpp
  utils/DateTestUnit.hpp
  utils/BlobTestUnit.hpp
  utils/BlobTestUnit.cpp
  utils/VarCharTestUnit.hpp
  utils/VarCharTestUnit.cpp
  utils/FactoryTestUnit.hpp
  utils/FactoryTestUnit.cpp
  utils/StringTestUnit.cpp
  utils/StringTestUnit.hpp
  utils/AnyTestUnit.cpp
  utils/AnyTestUnit.hpp
  utils/SequencerTestUnit.cpp
  utils/SequencerTestUnit.hpp
)

SET (TEST_HEADER Item.hpp has_many_list.hpp)

SET (TEST_OBJECT_SOURCES
#  object/ObjectPrototypeTestUnit.cpp
#  object/ObjectPrototypeTestUnit.hpp
  object/ObjectStoreTestUnit.cpp
  object/ObjectStoreTestUnit.hpp
#  object/PrototypeTreeTest.cpp
#  object/PrototypeTreeTest.hpp
#  object/PrimaryKeyUnitTest.cpp
#  object/PrimaryKeyUnitTest.hpp
#  object/HasManyVectorUnitTest.cpp
#  object/HasManyVectorUnitTest.hpp
#  object/HasManyListUnitTest.cpp
#  object/HasManyListUnitTest.hpp
#  object/ObjectTransactiontestUnit.cpp
#  object/ObjectTransactiontestUnit.hpp
  object/RelationTestUnit.cpp
  object/RelationTestUnit.hpp)

SET (TEST_UNIT_SOURCES
  unit/FirstTestUnit.hpp
  unit/SecondTestUnit.hpp
  unit/TestSuiteTestUnit.cpp
  unit/TestSuiteTestUnit.hpp)

SET (TEST_JSON_SOURCES
  json/JsonTestUnit.hpp
  json/JsonTestUnit.cpp
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
  sql/ValueUnitTest.hpp)

SET (TEST_ORM_SOURCES
  orm/TransactionTestUnit.cpp
  orm/TransactionTestUnit.hpp
  orm/OrmTestUnit.cpp
  orm/OrmTestUnit.hpp
  orm/OrmReloadTestUnit.cpp
  orm/OrmReloadTestUnit.hpp
  orm/OrmRelationTestUnit.cpp
  orm/OrmRelationTestUnit.hpp
)

SET (TEST_SOURCES test_matador.cpp)

ADD_EXECUTABLE(test_matador
  ${TEST_SOURCES}
  ${TEST_HEADER}
#  ${TEST_TOOLS_SOURCES}
  ${TEST_OBJECT_SOURCES}
#  ${TEST_UNIT_SOURCES}
#  ${TEST_ORM_SOURCES}
#  ${TEST_SQL_SOURCES}
)

SET(MYSQL_CONNECTION_STRING "mysql://root:root@127.0.0.1/oos_test" CACHE STRING "mysql connection string")
IF (WIN32)
  SET(MSSQL_CONNECTION_STRING "mssql://sascha:sascha@(local)\\\\SQLEXPRESS/test_oos (SQL Server)" CACHE STRING "mssql connection string")
ELSE()
  SET(MSSQL_CONNECTION_STRING "mssql://sascha:sascha@192.168.27.58/test (FreeTDS)" CACHE STRING "mssql connection string")
#  SET(MSSQL_CONNECTION_STRING "mssql://sascha:sascha@192.168.178.31\\\\SQLEXPRESS/test_matador (FreeTDS)" CACHE STRING "mssql connection string")
ENDIF()
SET(SQLITE_CONNECTION_STRING "sqlite://test.sqlite" CACHE STRING "sqlite connection string")

MESSAGE(STATUS "mysql connection string: ${MYSQL_CONNECTION_STRING}")
MESSAGE(STATUS "mssql connection string: ${MSSQL_CONNECTION_STRING}")
MESSAGE(STATUS "sqlite connection string: ${SQLITE_CONNECTION_STRING}")

CONFIGURE_FILE(connections.hpp.in ${PROJECT_BINARY_DIR}/connections.hpp @ONLY IMMEDIATE)

SET(DB_LIBRARIES)

IF(ODBC_FOUND)
  LIST(APPEND DB_LIBRARIES ${ODBC_LIBRARY})
ENDIF()

IF(MYSQL_FOUND)
  LIST(APPEND DB_LIBRARIES ${MYSQL_LIBRARY})
ENDIF()

IF(SQLITE3_FOUND)
  LIST(APPEND DB_LIBRARIES ${SQLITE3_LIBRARY})
ENDIF()

TARGET_LINK_LIBRARIES(test_matador
  matador-unit
  matador-utils
  matador-object
  matador-sql
  matador-orm
  ${CMAKE_DL_LIBS}
  ${DB_LIBRARIES}
)

# Group source files for IDE source explorers (e.g. Visual Studio)
SOURCE_GROUP("object" FILES ${TEST_OBJECT_SOURCES})
SOURCE_GROUP("utils" FILES ${TEST_TOOLS_SOURCES})
SOURCE_GROUP("json" FILES ${TEST_JSON_SOURCES})
SOURCE_GROUP("unit" FILES ${TEST_UNIT_SOURCES})
SOURCE_GROUP("orm" FILES ${TEST_ORM_SOURCES})
SOURCE_GROUP("sql" FILES ${TEST_SQL_SOURCES})
SOURCE_GROUP("main" FILES ${TEST_SOURCES})

MESSAGE(STATUS "Current binary dir: ${CMAKE_CURRENT_BINARY_DIR}")

ADD_TEST(test_matador_all ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_matador exec all)
