SET(OBJECT_SOURCES
  object/object.cpp
  object/object_creator.cpp
  object/object_deleter.cpp
  object/object_container.cpp
  object/object_ptr.cpp
  object/object_store.cpp
  object/object_proxy.cpp
  object/object_serializer.cpp
  object/object_convert.cpp
  object/prototype_node.cpp
  object/attribute_serializer.cpp
)

FILE(GLOB OBJECT_HEADER RELATIVE ${PROJECT_SOURCE_DIR} "${PROJECT_SOURCE_DIR}/include/object/*.hpp")

SET(OBJECT_INSTALL_HEADER
  ${PROJECT_SOURCE_DIR}/include/object/attribute_serializer.hpp
  ${PROJECT_SOURCE_DIR}/include/object/object.hpp
  ${PROJECT_SOURCE_DIR}/include/object/object_exception.hpp
  ${PROJECT_SOURCE_DIR}/include/object/object_ptr.hpp
  ${PROJECT_SOURCE_DIR}/include/object/object_store.hpp
  ${PROJECT_SOURCE_DIR}/include/object/object_container.hpp
  ${PROJECT_SOURCE_DIR}/include/object/object_convert.hpp
  ${PROJECT_SOURCE_DIR}/include/object/object_list.hpp
  ${PROJECT_SOURCE_DIR}/include/object/object_vector.hpp
  ${PROJECT_SOURCE_DIR}/include/object/linked_object_list.hpp
  ${PROJECT_SOURCE_DIR}/include/object/object_view.hpp
  ${PROJECT_SOURCE_DIR}/include/object/object_proxy.hpp
  ${PROJECT_SOURCE_DIR}/include/object/prototype_node.hpp
  ${PROJECT_SOURCE_DIR}/include/object/object_observer.hpp
  ${PROJECT_SOURCE_DIR}/include/object/object_expression.hpp
  ${PROJECT_SOURCE_DIR}/include/object/attribute_serializer.hpp
  ${PROJECT_SOURCE_DIR}/include/object/object_atomizer.hpp
  ${PROJECT_SOURCE_DIR}/include/object/object_atomizable.hpp
)

SET(TOOLS_SOURCES
  tools/byte_buffer.cpp
  tools/library.cpp
  tools/blob.cpp
  tools/varchar.cpp
  tools/sequencer.cpp
  tools/convert.cpp
)

SET(TOOLS_INSTALL_HEADER
  ${PROJECT_SOURCE_DIR}/include/tools/algorithm.hpp
  ${PROJECT_SOURCE_DIR}/include/tools/byte_buffer.hpp
  ${PROJECT_SOURCE_DIR}/include/tools/singleton.hpp
  ${PROJECT_SOURCE_DIR}/include/tools/library.hpp
  ${PROJECT_SOURCE_DIR}/include/tools/blob.hpp
  ${PROJECT_SOURCE_DIR}/include/tools/varchar.hpp
  ${PROJECT_SOURCE_DIR}/include/tools/sequencer.hpp
  ${PROJECT_SOURCE_DIR}/include/tools/factory.hpp
  ${PROJECT_SOURCE_DIR}/include/tools/string.hpp
  ${PROJECT_SOURCE_DIR}/include/tools/convert.hpp
  ${PROJECT_SOURCE_DIR}/include/tools/enable_if.hpp
  ${PROJECT_SOURCE_DIR}/include/tools/conditional.hpp
)

SET(JSON_SOURCE
  json/json_type.cpp
  json/json_value.cpp
  json/json_object.cpp
  json/json_string.cpp
  json/json_bool.cpp
  json/json_null.cpp
  json/json_number.cpp
  json/json_array.cpp
  json/json_exception.cpp
  json/json_parser.cpp
)

SET(JSON_INSTALL_HEADER
  ${PROJECT_SOURCE_DIR}/include/json/json.hpp
  ${PROJECT_SOURCE_DIR}/include/json/json_type.hpp
  ${PROJECT_SOURCE_DIR}/include/json/json_value.hpp
  ${PROJECT_SOURCE_DIR}/include/json/json_object.hpp
  ${PROJECT_SOURCE_DIR}/include/json/json_string.hpp
  ${PROJECT_SOURCE_DIR}/include/json/json_bool.hpp
  ${PROJECT_SOURCE_DIR}/include/json/json_null.hpp
  ${PROJECT_SOURCE_DIR}/include/json/json_number.hpp
  ${PROJECT_SOURCE_DIR}/include/json/json_array.hpp
  ${PROJECT_SOURCE_DIR}/include/json/json_exception.hpp
  ${PROJECT_SOURCE_DIR}/include/json/json_parser.hpp
  ${PROJECT_SOURCE_DIR}/include/json/generic_json_parser.hpp
)

SET(UNIT_SOURCES
  unit/test_suite.cpp
  unit/unit_exception.cpp
  unit/unit_test.cpp
)

SET(UNIT_HEADER
  ${PROJECT_SOURCE_DIR}/include/unit/test_suite.hpp
  ${PROJECT_SOURCE_DIR}/include/unit/unit_exception.hpp
  ${PROJECT_SOURCE_DIR}/include/unit/unit_test.hpp
)

SET(DATABASE_SOURCES
  database/action.cpp
  database/condition.cpp
  database/session.cpp
  database/database.cpp
  database/database_exception.cpp
  database/database_factory.cpp
  database/database_sequencer.cpp
  database/memory_database.cpp
  database/transaction.cpp
  database/transaction_helper.cpp
  database/result.cpp
  database/row.cpp
  database/statement.cpp
  database/statement_creator.cpp
  database/table.cpp
  database/sql.cpp
  database/query.cpp
  database/query_create.cpp
  database/query_select.cpp
  database/query_insert.cpp
  database/query_update.cpp
)

SET(DATABASE_HEADER
  ${PROJECT_SOURCE_DIR}/include/database/action.hpp
  ${PROJECT_SOURCE_DIR}/include/database/condition.hpp
  ${PROJECT_SOURCE_DIR}/include/database/database.hpp
  ${PROJECT_SOURCE_DIR}/include/database/database_factory.hpp
  ${PROJECT_SOURCE_DIR}/include/database/memory_database.hpp
  ${PROJECT_SOURCE_DIR}/include/database/database_sequencer.hpp
  ${PROJECT_SOURCE_DIR}/include/database/transaction_helper.hpp
  ${PROJECT_SOURCE_DIR}/include/database/result.hpp
  ${PROJECT_SOURCE_DIR}/include/database/row.hpp
  ${PROJECT_SOURCE_DIR}/include/database/value.hpp
  ${PROJECT_SOURCE_DIR}/include/database/statement.hpp
  ${PROJECT_SOURCE_DIR}/include/database/statement_creator.hpp
  ${PROJECT_SOURCE_DIR}/include/database/table.hpp
  ${PROJECT_SOURCE_DIR}/include/database/sql.hpp
  ${PROJECT_SOURCE_DIR}/include/database/query.hpp
  ${PROJECT_SOURCE_DIR}/include/database/query_create.hpp
  ${PROJECT_SOURCE_DIR}/include/database/query_select.hpp
  ${PROJECT_SOURCE_DIR}/include/database/query_insert.hpp
  ${PROJECT_SOURCE_DIR}/include/database/query_update.hpp
  ${PROJECT_SOURCE_DIR}/include/database/token.hpp
)

SET(DATABASE_INSTALL_HEADER
  ${PROJECT_SOURCE_DIR}/include/database/session.hpp
  ${PROJECT_SOURCE_DIR}/include/database/database_exception.hpp
  ${PROJECT_SOURCE_DIR}/include/database/transaction.hpp
)

ADD_LIBRARY(oos SHARED
  ${OBJECT_SOURCES}
  ${TOOLS_SOURCES}
  ${JSON_SOURCE}
  ${UNIT_SOURCES}
  ${DATABASE_SOURCES}
)

# Set the build version (VERSION) and the API version (SOVERSION)
SET_TARGET_PROPERTIES(oos
                      PROPERTIES
                      VERSION 0.1.0
                      SOVERSION 1)

ADD_SUBDIRECTORY(database)

# Group source files for IDE source explorers (e.g. Visual Studio)
SOURCE_GROUP("object" FILES ${OBJECT_SOURCES} ${OBJECT_HEADER})
SOURCE_GROUP("tools" FILES ${TOOLS_SOURCES} ${TOOLS_INSTALL_HEADER})
SOURCE_GROUP("json" FILES ${JSON_SOURCE} ${JSON_INSTALL_HEADER})
SOURCE_GROUP("unit" FILES ${UNIT_SOURCES} ${UNIT_HEADER})
SOURCE_GROUP("database" FILES ${DATABASE_SOURCES} ${DATABASE_HEADER} ${DATABASE_INSTALL_HEADER})

#FOREACH(inc ${OBJECT_HEADER})
#  MESSAGE(STATUS "object include: ${inc}")
#ENDFOREACH(inc)

INSTALL(
	TARGETS oos
	ARCHIVE	DESTINATION lib
	LIBRARY DESTINATION lib
	COMPONENT libraries
)

INSTALL(
	TARGETS oos
	DESTINATION lib
	COMPONENT libraries
)

INSTALL(
	FILES ${OBJECT_INSTALL_HEADER}
	DESTINATION include/oos/object
	COMPONENT headers
)

INSTALL(
	FILES ${DATABASE_INSTALL_HEADER}
	DESTINATION include/oos/database
	COMPONENT headers
)

INSTALL(
	FILES ${TOOLS_INSTALL_HEADER}
	DESTINATION include/oos/tools
	COMPONENT headers
)

INSTALL(
	FILES ${JSON_INSTALL_HEADER}
	DESTINATION include/oos/json
	COMPONENT headers
)

INSTALL(
	FILES ${UNIT_HEADER}
	DESTINATION include/oos/unit
	COMPONENT headers
)
