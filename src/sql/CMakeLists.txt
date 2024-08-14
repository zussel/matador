set(SOURCES
  any_type_to_string_visitor.cpp
  backend_provider.cpp
  basic_condition.cpp
  column.cpp
  column_definition.cpp
  column_definition_generator.cpp
  column_generator.cpp
  condition.cpp
  connection.cpp
  connection_impl.cpp
  connection_info.cpp
  dialect.cpp
  dialect_builder.cpp
  entity_query_builder.cpp
  field.cpp
  fk_value_extractor.cpp
  key_value_generator.cpp
  key_value_pair.cpp
  noop_connection.cpp
  object_parameter_binder.cpp
  placeholder_generator.cpp
  query.cpp
  query_builder.cpp
  query_compiler.cpp
  query_intermediates.cpp
  query_part.cpp
  query_parts.cpp
  query_result.cpp
  query_result_impl.cpp
  query_result_reader.cpp
  record.cpp
  result_parameter_binder.cpp
  schema.cpp
  session.cpp
  sql_logger.cpp
  statement.cpp
  statement_cache.cpp
  statement_impl.cpp
  table_definition.cpp
  value.cpp
  value_extractor.cpp
  placeholder_key_value_generator.cpp
)

set(HEADER
  ../../include/matador/sql/any_type_to_string_visitor.hpp
  ../../include/matador/sql/any_type_to_visitor.hpp
  ../../include/matador/sql/backend_provider.hpp
  ../../include/matador/sql/basic_condition.hpp
  ../../include/matador/sql/basic_sql_logger.hpp
  ../../include/matador/sql/column_definition.hpp
  ../../include/matador/sql/column_definition_generator.hpp
  ../../include/matador/sql/column_generator.hpp
  ../../include/matador/sql/condition.hpp
  ../../include/matador/sql/connection.hpp
  ../../include/matador/sql/connection_impl.hpp
  ../../include/matador/sql/connection_info.hpp
  ../../include/matador/sql/connection_pool.hpp
  ../../include/matador/sql/dialect.hpp
  ../../include/matador/sql/dialect_builder.hpp
  ../../include/matador/sql/entity_query_builder.hpp
  ../../include/matador/sql/field.hpp
  ../../include/matador/sql/fk_value_extractor.hpp
  ../../include/matador/sql/has_many_to_many_relation.hpp
  ../../include/matador/sql/identifier_binder.hpp
  ../../include/matador/sql/key_value_generator.hpp
  ../../include/matador/sql/key_value_pair.hpp
  ../../include/matador/sql/noop_connection.hpp
  ../../include/matador/sql/object_parameter_binder.hpp
  ../../include/matador/sql/parameter_binder.hpp
  ../../include/matador/sql/placeholder_generator.hpp
  ../../include/matador/sql/query.hpp
  ../../include/matador/sql/query_builder.hpp
  ../../include/matador/sql/query_compiler.hpp
  ../../include/matador/sql/query_context.hpp
  ../../include/matador/sql/query_data.hpp
  ../../include/matador/sql/query_helper.hpp
  ../../include/matador/sql/query_intermediates.hpp
  ../../include/matador/sql/query_part.hpp
  ../../include/matador/sql/query_part_visitor.hpp
  ../../include/matador/sql/query_parts.hpp
  ../../include/matador/sql/query_result.hpp
  ../../include/matador/sql/query_result_impl.hpp
  ../../include/matador/sql/query_result_reader.hpp
  ../../include/matador/sql/record.hpp
  ../../include/matador/sql/result_parameter_binder.hpp
  ../../include/matador/sql/schema.hpp
  ../../include/matador/sql/session.hpp
  ../../include/matador/sql/sql_logger.hpp
  ../../include/matador/sql/statement.hpp
  ../../include/matador/sql/statement_cache.hpp
  ../../include/matador/sql/statement_impl.hpp
  ../../include/matador/sql/table.hpp
  ../../include/matador/sql/table_definition.hpp
  ../../include/matador/sql/to_value.hpp
  ../../include/matador/sql/value.hpp
  ../../include/matador/sql/value_extractor.hpp
  ../../include/matador/sql/placeholder_key_value_generator.hpp
  ../../include/matador/sql/dialect_token.hpp
)

ADD_LIBRARY(matador-query STATIC ${SOURCES} ${HEADER})

TARGET_LINK_LIBRARIES(matador-query matador-object matador-utils)

# Set the build version (VERSION) and the API version (SOVERSION)
SET_TARGET_PROPERTIES(matador-query
  PROPERTIES
  VERSION ${APP_VERSION}
  SOVERSION ${APP_MAJOR_VERSION})

SOURCE_GROUP("include\\matador\\sql" FILES ${HEADER})
SOURCE_GROUP("src\\matador\\sql" FILES ${SOURCES})

INSTALL(
  TARGETS matador-query
  ARCHIVE  DESTINATION lib
  LIBRARY DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  TARGETS matador-query
  DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  FILES ${HEADER}
  DESTINATION include/matador/sql
  COMPONENT headers
)
