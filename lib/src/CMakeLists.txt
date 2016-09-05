SET(OBJECT_SOURCES
  object/action.cpp
  object/object_store.cpp
  object/object_proxy.cpp
  object/object_serializer.cpp
  object/prototype_node.cpp
  object/identifier.cpp
  object/prototype_iterator.cpp
  object/object_holder.cpp
  object/transaction.cpp
  object/action_inserter.cpp
  object/action_remover.cpp
  object/insert_action.cpp
  object/update_action.cpp
  object/delete_action.cpp
  object/basic_identifier_serializer.cpp
  object/basic_has_many_item.cpp object/object_proxy_accessor.cpp)

SET(OBJECT_INSTALL_HEADER
  ${PROJECT_SOURCE_DIR}/include/object/action.hpp
  ${PROJECT_SOURCE_DIR}/include/object/attribute_serializer.hpp
  ${PROJECT_SOURCE_DIR}/include/object/generic_access.hpp
  ${PROJECT_SOURCE_DIR}/include/object/serializable.hpp
  ${PROJECT_SOURCE_DIR}/include/object/object_exception.hpp
  ${PROJECT_SOURCE_DIR}/include/object/object_ptr.hpp
  ${PROJECT_SOURCE_DIR}/include/object/object_store.hpp
  ${PROJECT_SOURCE_DIR}/include/object/object_container.hpp
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
  ${PROJECT_SOURCE_DIR}/include/object/serializable.hpp
  ${PROJECT_SOURCE_DIR}/include/object/identifier.hpp
  ${PROJECT_SOURCE_DIR}/include/object/primary_key_writer.hpp
)

SET(OBJECT_HEADER
  ../include/object/action.hpp
  ../include/object/attribute_serializer.hpp
  ../include/object/generic_access.hpp
  ../include/object/object_exception.hpp
  ../include/object/object_ptr.hpp
  ../include/object/object_store.hpp
  ../include/object/object_view.hpp
  ../include/object/object_proxy.hpp
  ../include/object/object_serializer.hpp
  ../include/object/prototype_node.hpp
  ../include/object/object_observer.hpp
  ../include/object/object_expression.hpp
  ../include/object/attribute_serializer.hpp
  ../include/object/prototype_iterator.hpp
  ../include/object/has_many.hpp
  ../include/object/has_one.hpp
  ../include/object/container_type_traits.hpp
  ../include/object/object_holder.hpp
  ../include/object/basic_has_many.hpp
  ../include/object/has_many_vector.hpp
  ../include/object/has_many_list.hpp
  ../include/object/has_many_set.hpp
  ../include/object/transaction.hpp
  ../include/object/action_inserter.hpp
  ../include/object/action_visitor.hpp
  ../include/object/action_remover.hpp
  ../include/object/insert_action.hpp
  ../include/object/update_action.hpp
  ../include/object/delete_action.hpp
  ../include/object/abstract_has_many.hpp
  ../include/object/basic_identifier_serializer.hpp
  ../include/object/has_many_item.hpp
  ../include/object/basic_has_many_item.hpp
  ../include/object/identifier_proxy_map.hpp
        ../include/object/object_proxy_accessor.hpp)

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
  tools/basic_identifier.cpp
  tools/serializer.cpp
)

SET(TOOLS_INSTALL_HEADER
  ${PROJECT_SOURCE_DIR}/include/tools/access.hpp
  ${PROJECT_SOURCE_DIR}/include/tools/algorithm.hpp
  ${PROJECT_SOURCE_DIR}/include/tools/cascade_type.hpp
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
  ${PROJECT_SOURCE_DIR}/include/tools/basic_identifier.hpp
)

SET(TOOLS_HEADER
  ../include/tools/access.hpp
  ../include/tools/algorithm.hpp
  ../include/tools/base_class.hpp
  ../include/tools/byte_buffer.hpp
  ../include/tools/cascade_type.hpp
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
  ../include/tools/serializer.hpp
  ../include/tools/basic_identifier.hpp
  ../include/tools/identifier.hpp
  ../include/tools/identifier_resolver.hpp
  ../include/tools/identifier_setter.hpp
  ../include/tools/identifiable_holder.hpp)

SET(SQL_SOURCES
  sql/condition.cpp
  sql/connection.cpp
  sql/connection_factory.cpp
  sql/result_impl.cpp
  sql/sql.cpp
  sql/statement_impl.cpp
  sql/row.cpp
  sql/typed_column_serializer.cpp
  sql/token.cpp
  sql/column.cpp
  sql/dialect_token.cpp
  sql/value.cpp
  sql/basic_dialect.cpp
  sql/column_serializer.cpp
  sql/value_serializer.cpp
  sql/value_column_serializer.cpp
  sql/field.cpp
  sql/query.cpp
  sql/basic_query.cpp
  sql/basic_dialect_compiler.cpp
  sql/basic_dialect_linker.cpp)

SET(SQL_HEADER
  ../include/sql/condition.hpp
  ../include/sql/connection.hpp
  ../include/sql/connection_factory.hpp
  ../include/sql/connection_impl.hpp
  ../include/sql/result.hpp
  ../include/sql/result_impl.hpp
  ../include/sql/memory_connection.hpp
  ../include/sql/query.hpp
  ../include/sql/sql.hpp
  ../include/sql/row.hpp
  ../include/sql/value.hpp
  ../include/sql/statement.hpp
  ../include/sql/statement_impl.hpp
  ../include/sql/types.hpp
  ../include/sql/token.hpp
  ../include/sql/sql_exception.hpp
  ../include/sql/field.hpp
  ../include/sql/basic_dialect.hpp
  ../include/sql/typed_column_serializer.hpp
  ../include/sql/column.hpp
  ../include/sql/dialect_token.hpp
  ../include/sql/column_serializer.hpp
  ../include/sql/value_serializer.hpp
  ../include/sql/value_column_serializer.hpp
  ../include/sql/commands.hpp
  ../include/sql/basic_query.hpp
  ../include/sql/token_list.hpp
  ../include/sql/token_visitor.hpp
  ../include/sql/basic_dialect_compiler.hpp
  ../include/sql/basic_dialect_linker.hpp)

SET(ORM_HEADERS
  ../include/orm/persistence.hpp
  ../include/orm/table.hpp
  ../include/orm/session.hpp
  ../include/orm/basic_table.hpp
  ../include/orm/identifier_binder.hpp
  ../include/orm/identifier_column_resolver.hpp
  ../include/orm/relation_table.hpp
  ../include/orm/relation_resolver.hpp
  ../include/orm/relation_item_appender.hpp)

SET(ORM_SOURCES
  orm/persistence.cpp
  orm/session.cpp
  orm/basic_table.cpp)

SET(JSON_SOURCES
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
  ../include/object/action_inserter.hpp)

SET(DATABASE_SOURCES
  database/action.cpp
  database/session.cpp
  database/database.cpp
  database/database_exception.cpp
  database/database_factory.cpp
  database/database_sequencer.cpp
  database/memory_database.cpp
  database/transaction.cpp
  database/transaction_helper.cpp
  database/statement_creator.cpp
  database/table.cpp
  database/table_reader.cpp
  database/identifier_binder.cpp
)

SET(DATABASE_HEADER
  ../include/database/action.hpp
  ../include/database/session.hpp
  ../include/database/database.hpp
  ../include/database/database_exception.hpp
  ../include/database/database_factory.hpp
  ../include/database/memory_database.hpp
  ../include/database/database_sequencer.hpp
  ../include/database/transaction.hpp
  ../include/database/transaction_helper.hpp
  ../include/database/table.hpp
  ../include/database/table_reader.hpp
  ../include/database/identifier_binder.hpp
)

SET(DATABASE_INSTALL_HEADER
  ${PROJECT_SOURCE_DIR}/include/sql/session.hpp
  ${PROJECT_SOURCE_DIR}/include/sql/database_exception.hpp
  ${PROJECT_SOURCE_DIR}/include/sql/query.hpp
  ${PROJECT_SOURCE_DIR}/include/sql/condition.hpp
  ${PROJECT_SOURCE_DIR}/include/sql/transaction.hpp
)

ADD_LIBRARY(oos SHARED
  ${OBJECT_SOURCES}
  ${OBJECT_HEADER}
  ${TOOLS_SOURCES}
  ${TOOLS_HEADER}
  ${ORM_SOURCES}
  ${ORM_HEADERS}
#  ${JSON_SOURCES}
#  ${JSON_HEADER}
  ${UNIT_SOURCES}
  ${UNIT_HEADER}
  ${SQL_SOURCES}
  ${SQL_HEADER}
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
SOURCE_GROUP("include\\sql" FILES ${SQL_HEADER})
SOURCE_GROUP("include\\sql" FILES ${DATABASE_HEADER} ${DATABASE_INSTALL_HEADER})
SOURCE_GROUP("src\\object" FILES ${OBJECT_SOURCES})
SOURCE_GROUP("src\\\\tools" FILES ${TOOLS_SOURCES})
SOURCE_GROUP("src\\json" FILES ${JSON_SOURCE})
SOURCE_GROUP("src\\unit" FILES ${UNIT_SOURCES})
SOURCE_GROUP("src\\sql" FILES ${SQL_SOURCES})
SOURCE_GROUP("src\\sql" FILES ${DATABASE_SOURCES})

#FOREACH(inc ${OBJECT_HEADER})
#  MESSAGE(STATUS "object include: ${inc}")
#ENDFOREACH(inc)

#FOREACH(inc ${OBJECT_SOURCES})
#  MESSAGE(STATUS "object source: ${inc}")
#ENDFOREACH(inc)

INSTALL(
  TARGETS oos
  ARCHIVE  DESTINATION lib
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
  DESTINATION include/oos/sql
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

INSTALL(
  FILES ${SQL_HEADER}
  DESTINATION include/oos/sql
  COMPONENT headers
)
