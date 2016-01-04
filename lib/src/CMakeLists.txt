SET(OBJECT_SOURCES
#		object/object_container.cpp
#		object/object_ptr.cpp
		object/object_store.cpp
		object/object_proxy.cpp
		object/object_serializer.cpp
		object/prototype_node.cpp
		object/prototype_tree.cpp
#		object/primary_key_analyzer.cpp
#		object/foreign_key_analyzer.cpp
		object/basic_identifier.cpp
		object/identifier.cpp
		object/prototype_iterator.cpp
		object/basic_object_holder.cpp)

SET(OBJECT_INSTALL_HEADER
  ${PROJECT_SOURCE_DIR}/include/object/attribute_serializer.hpp
  ${PROJECT_SOURCE_DIR}/include/object/generic_access.hpp
  ${PROJECT_SOURCE_DIR}/include/object/serializable.hpp
  ${PROJECT_SOURCE_DIR}/include/object/object_exception.hpp
  ${PROJECT_SOURCE_DIR}/include/object/object_ptr.hpp
  ${PROJECT_SOURCE_DIR}/include/object/object_store.hpp
  ${PROJECT_SOURCE_DIR}/include/object/object_container.hpp
  ${PROJECT_SOURCE_DIR}/include/object/object_list.hpp
  ${PROJECT_SOURCE_DIR}/include/object/object_vector.hpp
  ${PROJECT_SOURCE_DIR}/include/object/object_producer.hpp
  ${PROJECT_SOURCE_DIR}/include/object/linked_object_list.hpp
  ${PROJECT_SOURCE_DIR}/include/object/object_view.hpp
  ${PROJECT_SOURCE_DIR}/include/object/object_proxy.hpp
  ${PROJECT_SOURCE_DIR}/include/object/prototype_node.hpp
  ${PROJECT_SOURCE_DIR}/include/object/prototype_tree.hpp
  ${PROJECT_SOURCE_DIR}/include/object/object_observer.hpp
  ${PROJECT_SOURCE_DIR}/include/object/object_expression.hpp
  ${PROJECT_SOURCE_DIR}/include/object/attribute_serializer.hpp
  ${PROJECT_SOURCE_DIR}/include/object/object_atomizer.hpp
  ${PROJECT_SOURCE_DIR}/include/object/serializable.hpp
  ${PROJECT_SOURCE_DIR}/include/object/identifier.hpp
  ${PROJECT_SOURCE_DIR}/include/object/primary_key_writer.hpp
)

SET(OBJECT_HEADER
#		../include/object/attribute_counter.hpp
		../include/object/attribute_serializer.hpp
		../include/object/generic_access.hpp
		../include/object/object_exception.hpp
		../include/object/object_ptr.hpp
		../include/object/object_store.hpp
#		../include/object/object_container.hpp
#		../include/object/object_list.hpp
#		../include/object/object_vector.hpp
#		../include/object/object_producer.hpp
#		../include/object/linked_object_list.hpp
#		../include/object/object_view.hpp
		../include/object/object_proxy.hpp
		../include/object/object_serializer.hpp
		../include/object/prototype_node.hpp
		../include/object/prototype_tree.hpp
		../include/object/object_observer.hpp
#		../include/object/object_expression.hpp
#		../include/object/attribute_serializer.hpp
		../include/object/identifier.hpp
#		../include/object/primary_key_analyzer.hpp
#		../include/object/foreign_key_analyzer.hpp
		../include/object/identifier_resolver.hpp
		../include/object/identifier_setter.hpp
		../include/object/basic_identifier.hpp
		../include/object/prototype_iterator.hpp
		../include/object/base_class.hpp
		../include/object/has_many.hpp
		../include/object/has_one.hpp
		../include/object/container_type_traits.hpp
		../include/object/basic_object_holder.hpp ../include/object/cascade_type.hpp)

SET(TOOLS_SOURCES
  tools/byte_buffer.cpp
  tools/library.cpp
  tools/blob.cpp
  tools/calendar.cpp
  tools/date.cpp
  tools/time.cpp
  tools/varchar.cpp
  tools/sequencer.cpp
  tools/string.cpp
  tools/strptime.cpp
)

SET(TOOLS_INSTALL_HEADER
  ${PROJECT_SOURCE_DIR}/include/tools/algorithm.hpp
  ${PROJECT_SOURCE_DIR}/include/tools/byte_buffer.hpp
  ${PROJECT_SOURCE_DIR}/include/tools/singleton.hpp
  ${PROJECT_SOURCE_DIR}/include/tools/library.hpp
  ${PROJECT_SOURCE_DIR}/include/tools/blob.hpp
  ${PROJECT_SOURCE_DIR}/include/tools/date.hpp
  ${PROJECT_SOURCE_DIR}/include/tools/time.hpp
  ${PROJECT_SOURCE_DIR}/include/tools/varchar.hpp
  ${PROJECT_SOURCE_DIR}/include/tools/sequencer.hpp
  ${PROJECT_SOURCE_DIR}/include/tools/factory.hpp
  ${PROJECT_SOURCE_DIR}/include/tools/string.hpp
  ${PROJECT_SOURCE_DIR}/include/tools/strptime.hpp
  ${PROJECT_SOURCE_DIR}/include/tools/enable_if.hpp
  ${PROJECT_SOURCE_DIR}/include/tools/conditional.hpp
)

SET(TOOLS_HEADER
  ../include/tools/algorithm.hpp
  ../include/tools/byte_buffer.hpp
  ../include/tools/singleton.hpp
  ../include/tools/library.hpp
  ../include/tools/blob.hpp
  ../include/tools/calendar.h
  ../include/tools/date.hpp
  ../include/tools/time.hpp
  ../include/tools/varchar.hpp
  ../include/tools/sequencer.hpp
  ../include/tools/factory.hpp
  ../include/tools/string.hpp
  ../include/tools/strptime.hpp
  ../include/tools/enable_if.hpp
  ../include/tools/conditional.hpp
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
  ${PROJECT_SOURCE_DIR}/include/json/json_exception.hpp
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

SET(JSON_HEADER
  ../include/json/json.hpp
  ../include/json/json_type.hpp
  ../include/json/json_value.hpp
  ../include/json/json_object.hpp
  ../include/json/json_string.hpp
  ../include/json/json_bool.hpp
  ../include/json/json_null.hpp
  ../include/json/json_number.hpp
  ../include/json/json_array.hpp
  ../include/json/json_exception.hpp
  ../include/json/json_parser.hpp
  ../include/json/generic_json_parser.hpp
)

SET(UNIT_SOURCES
  unit/test_suite.cpp
  unit/unit_exception.cpp
  unit/unit_test.cpp
)

SET(UNIT_INSTALL_HEADER
  ${PROJECT_SOURCE_DIR}/include/unit/test_suite.hpp
  ${PROJECT_SOURCE_DIR}/include/unit/unit_exception.hpp
  ${PROJECT_SOURCE_DIR}/include/unit/unit_test.hpp
)

SET(UNIT_HEADER
		../include/unit/test_suite.hpp
		../include/unit/unit_exception.hpp
		../include/unit/unit_test.hpp
		../include/object/access.hpp)

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
  database/result_impl.cpp
  database/row.cpp
  database/statement_creator.cpp
  database/table.cpp
  database/table_reader.cpp
  database/sql.cpp
  database/query_create.cpp
  database/query_select.cpp
  database/query_insert.cpp
  database/query_update.cpp
  database/identifier_binder.cpp
  database/statement_impl.cpp)

SET(DATABASE_HEADER
  ../include/database/action.hpp
  ../include/database/condition.hpp
  ../include/database/session.hpp
  ../include/database/database.hpp
  ../include/database/database_exception.hpp
  ../include/database/database_factory.hpp
  ../include/database/memory_database.hpp
  ../include/database/database_sequencer.hpp
  ../include/database/transaction.hpp
  ../include/database/transaction_helper.hpp
  ../include/database/result.hpp
  ../include/database/result_impl.hpp
  ../include/database/row.hpp
  ../include/database/value.hpp
  ../include/database/types.hpp
  ../include/database/sql.hpp
  ../include/database/statement.hpp
  ../include/database/table.hpp
  ../include/database/table_reader.hpp
  ../include/database/query.hpp
  ../include/database/query_create.hpp
  ../include/database/query_select.hpp
  ../include/database/query_insert.hpp
  ../include/database/query_update.hpp
  ../include/database/token.hpp
  ../include/database/identifier_binder.hpp
  ../include/database/statement_impl.hpp)

SET(DATABASE_INSTALL_HEADER
  ${PROJECT_SOURCE_DIR}/include/database/session.hpp
  ${PROJECT_SOURCE_DIR}/include/database/database_exception.hpp
  ${PROJECT_SOURCE_DIR}/include/database/query.hpp
  ${PROJECT_SOURCE_DIR}/include/database/result.hpp
  ${PROJECT_SOURCE_DIR}/include/database/sql.hpp
  ${PROJECT_SOURCE_DIR}/include/database/condition.hpp
  ${PROJECT_SOURCE_DIR}/include/database/types.hpp
  ${PROJECT_SOURCE_DIR}/include/database/transaction.hpp
)

ADD_LIBRARY(oos SHARED
  ${OBJECT_SOURCES}
  ${OBJECT_HEADER}
  ${TOOLS_SOURCES}
  ${TOOLS_HEADER}
#  ${JSON_SOURCE}
#  ${JSON_HEADER}
  ${UNIT_SOURCES}
  ${UNIT_HEADER}
#  ${DATABASE_SOURCES}
#  ${DATABASE_HEADER}
)

TARGET_LINK_LIBRARIES(oos ${CMAKE_DL_LIBS})

# Set the build version (VERSION) and the API version (SOVERSION)
SET_TARGET_PROPERTIES(oos
                      PROPERTIES
                      VERSION 0.2.1
                      SOVERSION 0)

# Group source files for IDE source explorers (e.g. Visual Studio)
SOURCE_GROUP("include\\object" FILES ${OBJECT_HEADER})
SOURCE_GROUP("include\\tools" FILES ${TOOLS_HEADER})
SOURCE_GROUP("include\\json" FILES ${JSON_HEADER})
SOURCE_GROUP("include\\unit" FILES ${UNIT_HEADER})
SOURCE_GROUP("include\\database" FILES ${DATABASE_HEADER} ${DATABASE_INSTALL_HEADER})
SOURCE_GROUP("src\\object" FILES ${OBJECT_SOURCES})
SOURCE_GROUP("src\\\\tools" FILES ${TOOLS_SOURCES})
SOURCE_GROUP("src\\json" FILES ${JSON_SOURCE})
SOURCE_GROUP("src\\unit" FILES ${UNIT_SOURCES})
SOURCE_GROUP("src\\database" FILES ${DATABASE_SOURCES})

#FOREACH(inc ${OBJECT_HEADER})
#  MESSAGE(STATUS "object include: ${inc}")
#ENDFOREACH(inc)

#FOREACH(inc ${OBJECT_SOURCES})
#  MESSAGE(STATUS "object source: ${inc}")
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
	FILES ${UNIT_INSTALL_HEADER}
	DESTINATION include/oos/unit
	COMPONENT headers
)
