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
  database/SQLiteDatabaseTestUnit.cpp
  database/SQLiteDatabaseTestUnit.hpp
  database/DatabaseTestUnit.cpp
  database/DatabaseTestUnit.hpp
  database/MySQLDatabaseTestUnit.cpp
  database/MySQLDatabaseTestUnit.hpp
  database/MSSQLDatabaseTestUnit.cpp
  database/MSSQLDatabaseTestUnit.hpp
)

SET (TEST_SOURCES test_oos.cpp)

ADD_EXECUTABLE(test_oos
  ${TEST_SOURCES}
  ${TEST_TOOLS_SOURCES}
  ${TEST_OBJECT_SOURCES}
  ${TEST_UNIT_SOURCES}
  ${TEST_JSON_SOURCES}
  ${TEST_DATABASE_SOURCES}
)

SET(MYSQL_CONNECTION_STRING "mysql://root@127.0.0.1/oos_test" CACHE STRING "mysql connection string")
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

ADD_CUSTOM_COMMAND(TARGET test_oos POST_BUILD
                   COMMAND test_oos list brief > list.txt
                   COMMAND echo `pwd`)

# Group source files for IDE source explorers (e.g. Visual Studio)
SOURCE_GROUP("object" FILES ${TEST_OBJECT_SOURCES})
SOURCE_GROUP("tools" FILES ${TEST_TOOLS_SOURCES})
SOURCE_GROUP("json" FILES ${TEST_JSON_SOURCES})
SOURCE_GROUP("unit" FILES ${TEST_UNIT_SOURCES})
SOURCE_GROUP("database" FILES ${TEST_DATABASE_SOURCES})
SOURCE_GROUP("main" FILES ${TEST_SOURCES})

MESSAGE(STATUS "Current binary dir: ${CMAKE_CURRENT_BINARY_DIR}")

#SET(OOS_TESTS)
#FILE(STRINGS ${CMAKE_BINARY_DIR}/test/list.txt OOS_TESTS)

#FOREACH (T ${OOS_TESTS})
#  MESSAGE(STATUS "Adding test ${T}")
#  ADD_TEST(test_greeter_${T} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec ${T})
#ENDFOREACH(T)

#FILE(REMOVE ${CMAKE_BINARY_DIR}/test/list.txt)

# add tests
ADD_TEST(test_oos_convert_bool ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec convert:to_bool)
ADD_TEST(test_oos_convert_char_pointer ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec convert:to_char_pointer)
ADD_TEST(test_oos_convert_float ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec convert:to_float)
ADD_TEST(test_oos_convert_double ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec convert:to_double)
ADD_TEST(test_oos_convert_string ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec convert:to_string)
ADD_TEST(test_oos_convert_varchar ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec convert:to_varchar)
ADD_TEST(test_oos_convert_char ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec convert:to_char)
ADD_TEST(test_oos_convert_short ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec convert:to_short)
ADD_TEST(test_oos_convert_int ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec convert:to_int)
ADD_TEST(test_oos_convert_long ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec convert:to_long)
ADD_TEST(test_oos_convert_unsigned_char ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec convert:to_unsigned_char)
ADD_TEST(test_oos_convert_unsigned_short ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec convert:to_unsigned_short)
ADD_TEST(test_oos_convert_unsigned_int ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec convert:to_unsigned_int)
ADD_TEST(test_oos_convert_unsigned_long ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec convert:to_unsigned_long)
ADD_TEST(test_oos_factory_create ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec factory:create)
ADD_TEST(test_oos_factory_insert ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec factory:insert)
ADD_TEST(test_oos_factory_list ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec factory:list)
ADD_TEST(test_oos_actory_produce ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec factory:produce)
ADD_TEST(test_oos_first_sub1 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec first:sub1)
ADD_TEST(test_oos_first_sub2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec first:sub2)
ADD_TEST(test_oos_first_sub3 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec first:sub3)
ADD_TEST(test_oos_json_access ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec json:access)
ADD_TEST(test_oos_json_create ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec json:create)
ADD_TEST(test_oos_json_number ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec json:number)
ADD_TEST(test_oos_json_parser ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec json:parser)
ADD_TEST(test_oos_json_simple ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec json:simple)
ADD_TEST(test_oos_json_string ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec json:string)
ADD_TEST(test_oos_list_direct_ref ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec list:direct_ref)
ADD_TEST(test_oos_list_int ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec list:int)
ADD_TEST(test_oos_list_linked_int ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec list:linked_int)
ADD_TEST(test_oos_list_linked_ptr ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec list:linked_ptr)
ADD_TEST(test_oos_list_linked_ref ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec list:linked_ref)
ADD_TEST(test_oos_list_linked_ref ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec list:linked_ref)
ADD_TEST(test_oos_list_ref ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec list:ref)
ADD_TEST(test_oos_prototype_empty ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec prototype:empty)
ADD_TEST(test_oos_prototype_find ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec prototype:find)
ADD_TEST(test_oos_prototype_hierarchy ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec prototype:hierarchy)
ADD_TEST(test_oos_prototype_iterator ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec prototype:iterator)
ADD_TEST(test_oos_prototype_one ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec prototype:one)
ADD_TEST(test_oos_prototype_relation ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec prototype:relation)
ADD_TEST(test_oos_second_big ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec second:big)
ADD_TEST(test_oos_second_small ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec second:small)
ADD_TEST(test_oos_store_version ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec store:version)
ADD_TEST(test_oos_store_clear ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec store:clear)
ADD_TEST(test_oos_store_delete ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec store:delete)
ADD_TEST(test_oos_store_expression ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec store:expression)
ADD_TEST(test_oos_store_generic ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec store:generic)
ADD_TEST(test_oos_store_get ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec store:get)
ADD_TEST(test_oos_store_hierarchy ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec store:hierarchy)
ADD_TEST(test_oos_store_multiple_object_with_sub ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec store:multiple_object_with_sub)
ADD_TEST(test_oos_store_multiple_simple ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec store:multiple_simple)
ADD_TEST(test_oos_store_ref_ptr_counter ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec store:ref_ptr_counter)
ADD_TEST(test_oos_store_serializer ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec store:serializer)
ADD_TEST(test_oos_store_set ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec store:set)
ADD_TEST(test_oos_store_simple ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec store:simple)
ADD_TEST(test_oos_store_structure ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec store:structure)
ADD_TEST(test_oos_store_sub_delete ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec store:sub_delete)
ADD_TEST(test_oos_store_view ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec store:view)
ADD_TEST(test_oos_store_with_sub ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec store:with_sub)
ADD_TEST(test_oos_varchar_assign ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec varchar:assign)
ADD_TEST(test_oos_varchar_copy ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec varchar:copy)
ADD_TEST(test_oos_varchar_create ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec varchar:create)
ADD_TEST(test_oos_vector_direct_ref ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec vector:direct_ref)
ADD_TEST(test_oos_vector_int ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec vector:int)
ADD_TEST(test_oos_vector_ptr ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec vector:ptr)
ADD_TEST(test_oos_vector_ref ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec vector:ref)

IF(SQLITE3_FOUND)
  ADD_TEST(test_oos_sqlite_open_close ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec sqlite:open_close)
  ADD_TEST(test_oos_sqlite_create_drop ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec sqlite:create_drop)
  ADD_TEST(test_oos_sqlite_reopen ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec sqlite:reopen)
  ADD_TEST(test_oos_sqlite_insert ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec sqlite:insert)
  ADD_TEST(test_oos_sqlite_update ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec sqlite:update)
  ADD_TEST(test_oos_sqlite_delete ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec sqlite:delete)
  ADD_TEST(test_oos_sqlite_simple ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec sqlite:simple)
  ADD_TEST(test_oos_sqlite_complex ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec sqlite:complex)
  ADD_TEST(test_oos_sqlite_list ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec sqlite:list)
  ADD_TEST(test_oos_sqlite_vector ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec sqlite:vector)
  ADD_TEST(test_oos_sqlite_reload ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec sqlite:reload)
  ADD_TEST(test_oos_sqlite_reload_container ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec sqlite:container)
ELSE()
  MESSAGE("skipping SQLite tests")
ENDIF()
IF(MYSQL_FOUND)
  ADD_TEST(test_oos_mysql_open_close ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec mysql:open_close)
  ADD_TEST(test_oos_mysql_create_drop ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec mysql:create_drop)
  ADD_TEST(test_oos_mysql_reopen ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec mysql:reopen)
  ADD_TEST(test_oos_mysql_insert ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec mysql:insert)
  ADD_TEST(test_oos_mysql_update ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec mysql:update)
  ADD_TEST(test_oos_mysql_delete ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec mysql:delete)
  ADD_TEST(test_oos_mysql_simple ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec mysql:simple)
  ADD_TEST(test_oos_mysql_complex ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec mysql:complex)
  ADD_TEST(test_oos_mysql_list ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec mysql:list)
  ADD_TEST(test_oos_mysql_vector ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec mysql:vector)
  ADD_TEST(test_oos_mysql_reload ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec mysql:reload)
  ADD_TEST(test_oos_mysql_reload_container ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_oos exec mysql:container)
ELSE()
  MESSAGE("skipping MySQL tests")
ENDIF()
