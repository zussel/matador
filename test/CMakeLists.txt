#ADD_SUBDIRECTORY(tools)
#ADD_SUBDIRECTORY(database)
#ADD_SUBDIRECTORY(object)
#ADD_SUBDIRECTORY(json)
#ADD_SUBDIRECTORY(unit)

SET (TEST_TOOLS_SOURCES
  tools/BlobTestUnit.hpp
  tools/BlobTestUnit.cpp
  tools/ConvertTestUnit.hpp
  tools/ConvertTestUnit.cpp
  tools/VarCharTestUnit.hpp
  tools/VarCharTestUnit.cpp
  tools/FactoryTestUnit.hpp
  tools/FactoryTestUnit.cpp
)

SET (TEST_HEADER Item.hpp)

SET (TEST_OBJECT_SOURCES
  object/ObjectPrototypeTestUnit.cpp
  object/ObjectPrototypeTestUnit.hpp
  object/ObjectStoreTestUnit.cpp
  object/ObjectStoreTestUnit.hpp
  object/ObjectListTestUnit.cpp
  object/ObjectListTestUnit.hpp
  object/ObjectVectorTestUnit.cpp
  object/ObjectVectorTestUnit.hpp
  object/PrototypeTreeTest.cpp
  object/PrototypeTreeTest.hpp
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
)

SET (TEST_SOURCES test_oos.cpp)

ADD_EXECUTABLE(test_oos
  ${TEST_SOURCES}
  ${TEST_HEADER}
  ${TEST_TOOLS_SOURCES}
  ${TEST_OBJECT_SOURCES}
  ${TEST_UNIT_SOURCES}
  ${TEST_JSON_SOURCES}
  ${TEST_DATABASE_SOURCES}
)

SET(MYSQL_CONNECTION_STRING "mysql://root:root@127.0.0.1/oos_test" CACHE STRING "mysql connection string")
IF (WIN32)
  SET(MSSQL_CONNECTION_STRING "mssql://sascha@127.0.0.1/SQLEXPRESS (SQL Server)" CACHE STRING "mssql connection string")
ELSE()
  SET(MSSQL_CONNECTION_STRING "mssql://sascha@192.168.27.89/SQLEXPRESS (FreeTDS)" CACHE STRING "mssql connection string")
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

# convert tests
SET(convert
  to_bool
  to_char_pointer
  to_float
  to_double
  to_string
  to_varchar
  to_char
  to_short
  to_int
  to_long
  to_unsigned_char
  to_unsigned_short
  to_unsigned_int
  to_unsigned_long
)

# factory tests
SET(factory
  create
  insert
  list
  produce
)

# first tests
SET(first
  sub1
  sub2
  sub3
)

# second tests
SET(second
  small
  big
)

# json tests
SET(json
  access
  create
  bool
  number
  parser
  array
  simple
  string
)

# list tests
SET(list
  direct_ref
  int
  ptr
  ref
  linked_int
  linked_ptr
  linked_ref
)

# vector tests
SET(vector
  direct_ref
  int
  ptr
  ref
)

# prototype tests
SET(prototype
  empty
  find
  hierarchy
  iterator
  one
  relation
  size
  parent_of
)

# prototype tree tests
SET(tree
  empty
  insert
  insert_template
  find
  remove
  erase
  clear
  container
  decrement
  child_of
  traverse
  const_traverse
)

# object store tests
SET(store
  version
  clear
  delete
  expression
  generic
  get
  hierarchy
  multiple_object_with_sub
  multiple_simple
  ref_ptr_counter
  serializer
  set
  simple
#  structure
  sub_delete
  view
  with_sub
  insert
  remove
)

# varchar tests
SET(varchar
  assign
  copy
  create
)

SET(TESTUNITS)

LIST(APPEND TESTUNITS convert)
LIST(APPEND TESTUNITS factory)
LIST(APPEND TESTUNITS first)
LIST(APPEND TESTUNITS second)
LIST(APPEND TESTUNITS json)
LIST(APPEND TESTUNITS list)
LIST(APPEND TESTUNITS vector)
LIST(APPEND TESTUNITS tree)
LIST(APPEND TESTUNITS prototype)
LIST(APPEND TESTUNITS store)
LIST(APPEND TESTUNITS varchar)

SET(database
  open_close
  create_drop
  reopen
  insert
  update
  delete
  datatypes
  simple
  complex
  list
  vector
  reload
  reload_container
)
  
IF(SQLITE3_FOUND AND OOS_SQLITE3)
  SET(sqlite ${database})
  LIST(APPEND TESTUNITS sqlite)
ELSE()
  MESSAGE(STATUS "skipping SQLite tests")
ENDIF()

IF(MYSQL_FOUND AND OOS_MYSQL)
  SET(mysql ${database})
  LIST(APPEND TESTUNITS mysql)
ELSE()
  MESSAGE(STATUS "skipping MySQL tests")
ENDIF()

IF(ODBC_FOUND AND OOS_ODBC)
  SET(mssql ${database})
  LIST(APPEND TESTUNITS mssql)
ELSE()
  MESSAGE(STATUS "skipping MSSQL tests")
ENDIF()

FOREACH(unit ${TESTUNITS})
  FOREACH(test ${${unit}})
    ADD_TEST(test_oos_${unit}_${test} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec ${unit}:${test})
#    MESSAGE(STATUS "add test ${unit}:${test}")
  ENDFOREACH(test)
ENDFOREACH(unit)
