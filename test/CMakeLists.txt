#ADD_SUBDIRECTORY(tools)
#ADD_SUBDIRECTORY(database)
#ADD_SUBDIRECTORY(serializable)
#ADD_SUBDIRECTORY(json)
#ADD_SUBDIRECTORY(unit)

SET (TEST_TOOLS_SOURCES
  tools/TimeTestUnit.cpp
  tools/TimeTestUnit.hpp
  tools/DateTestUnit.cpp
  tools/DateTestUnit.hpp
  tools/BlobTestUnit.hpp
  tools/BlobTestUnit.cpp
  tools/VarCharTestUnit.hpp
  tools/VarCharTestUnit.cpp
  tools/FactoryTestUnit.hpp
  tools/FactoryTestUnit.cpp
  tools/StringTestUnit.cpp
  tools/StringTestUnit.hpp
)

SET (TEST_HEADER Item.hpp)

SET (TEST_OBJECT_SOURCES
  object/ObjectPrototypeTestUnit.cpp
  object/ObjectPrototypeTestUnit.hpp
  object/ObjectStoreTestUnit.cpp
  object/ObjectStoreTestUnit.hpp
#  object/ObjectListTestUnit.cpp
#  object/ObjectListTestUnit.hpp
#  object/ObjectVectorTestUnit.cpp
#  object/ObjectVectorTestUnit.hpp
  object/PrototypeTreeTest.cpp
  object/PrototypeTreeTest.hpp
  object/PrimaryKeyUnitTest.cpp
  object/PrimaryKeyUnitTest.hpp
  object/HasManyUnitTest.cpp
  object/HasManyUnitTest.hpp
)

SET (TEST_UNIT_SOURCES
  unit/FirstTestUnit.hpp
  unit/SecondTestUnit.hpp
)

SET (TEST_JSON_SOURCES
  json/JsonTestUnit.hpp
  json/JsonTestUnit.cpp
)

SET (TEST_DATABASE_SOURCES
  database/DatabaseTestUnit.cpp
  database/DatabaseTestUnit.hpp
  database/SessionTestUnit.hpp
  database/SessionTestUnit.cpp
  database/TransactionTestUnit.cpp
  database/TransactionTestUnit.hpp
        database/SQLTestUnit.cpp database/SQLTestUnit.hpp)

SET (TEST_SOURCES test_oos.cpp object/PrimaryKeyUnitTest.cpp object/PrimaryKeyUnitTest.hpp)

ADD_EXECUTABLE(test_oos
  ${TEST_SOURCES}
  ${TEST_HEADER}
  ${TEST_TOOLS_SOURCES}
  ${TEST_OBJECT_SOURCES}
  ${TEST_UNIT_SOURCES}
#  ${TEST_JSON_SOURCES}
#  ${TEST_DATABASE_SOURCES}
)

SET(MYSQL_CONNECTION_STRING "mysql://root:root@127.0.0.1/oos_test" CACHE STRING "mysql connection string")
IF (WIN32)
  SET(MSSQL_CONNECTION_STRING "mssql://sascha@127.0.0.1/SQLEXPRESS (SQL Server)" CACHE STRING "mssql connection string")
ELSE()
  SET(MSSQL_CONNECTION_STRING "mssql://sascha:sascha@192.168.27.58/test (FreeTDS)" CACHE STRING "mssql connection string")
ENDIF()
SET(SQLITE_CONNECTION_STRING "sqlite://test.sqlite" CACHE STRING "sqlite connection string")

MESSAGE(STATUS "mysql connection string: ${MYSQL_CONNECTION_STRING}")
MESSAGE(STATUS "mssql connection string: ${MSSQL_CONNECTION_STRING}")
MESSAGE(STATUS "sqlite connection string: ${SQLITE_CONNECTION_STRING}")

CONFIGURE_FILE(connections.hpp.in ${PROJECT_BINARY_DIR}/connections.hpp @ONLY IMMEDIATE)

TARGET_LINK_LIBRARIES(test_oos oos ${CMAKE_DL_LIBS})

# Group source files for IDE source explorers (e.g. Visual Studio)
SOURCE_GROUP("object" FILES ${TEST_OBJECT_SOURCES})
SOURCE_GROUP("tools" FILES ${TEST_TOOLS_SOURCES})
SOURCE_GROUP("json" FILES ${TEST_JSON_SOURCES})
SOURCE_GROUP("unit" FILES ${TEST_UNIT_SOURCES})
SOURCE_GROUP("database" FILES ${TEST_DATABASE_SOURCES})
SOURCE_GROUP("main" FILES ${TEST_SOURCES})

MESSAGE(STATUS "Current binary dir: ${CMAKE_CURRENT_BINARY_DIR}")

ADD_TEST(test_oos_all ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec all)
