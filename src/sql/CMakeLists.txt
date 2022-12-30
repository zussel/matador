SET(SOURCES
  condition.cpp
  connection.cpp
  connection_factory.cpp
  result_impl.cpp
  sql.cpp
  statement_impl.cpp
  row.cpp
  typed_column_serializer.cpp
  token.cpp
  column.cpp
  dialect_token.cpp
  value.cpp
  basic_dialect.cpp
  column_serializer.cpp
  value_serializer.cpp
  value_column_serializer.cpp
  field.cpp
  query.cpp
  basic_query.cpp
  basic_dialect_compiler.cpp
  basic_dialect_linker.cpp
  type.cpp
  query_value_column_processor.cpp
  query_value_creator.cpp
  connection_impl.cpp
  value_visitor.cpp
  database_error.cpp
  sql_logger.cpp)

SET(HEADER
  ../../include/matador/sql/condition.hpp
  ../../include/matador/sql/connection.hpp
  ../../include/matador/sql/connection_factory.hpp
  ../../include/matador/sql/connection_impl.hpp
  ../../include/matador/sql/result.hpp
  ../../include/matador/sql/result_impl.hpp
  ../../include/matador/sql/memory_connection.hpp
  ../../include/matador/sql/query.hpp
  ../../include/matador/sql/sql.hpp
  ../../include/matador/sql/row.hpp
  ../../include/matador/sql/value.hpp
  ../../include/matador/sql/statement.hpp
  ../../include/matador/sql/statement_impl.hpp
  ../../include/matador/sql/types.hpp
  ../../include/matador/sql/token.hpp
  ../../include/matador/sql/sql_exception.hpp
  ../../include/matador/sql/field.hpp
  ../../include/matador/sql/basic_dialect.hpp
  ../../include/matador/sql/typed_column_serializer.hpp
  ../../include/matador/sql/column.hpp
  ../../include/matador/sql/dialect_token.hpp
  ../../include/matador/sql/column_serializer.hpp
  ../../include/matador/sql/value_serializer.hpp
  ../../include/matador/sql/value_column_serializer.hpp
  ../../include/matador/sql/commands.hpp
  ../../include/matador/sql/basic_query.hpp
  ../../include/matador/sql/token_list.hpp
  ../../include/matador/sql/token_visitor.hpp
  ../../include/matador/sql/basic_dialect_compiler.hpp
  ../../include/matador/sql/basic_dialect_linker.hpp
  ../../include/matador/sql/query_value_column_processor.hpp
  ../../include/matador/sql/query_value_creator.hpp
  ../../include/matador/sql/parameter_binder.hpp
  ../../include/matador/sql/value_visitor.hpp
  ../../include/matador/sql/database_error.hpp
  ../../include/matador/sql/basic_sql_logger.hpp
  ../../include/matador/sql/sql_logger.hpp
)

ADD_LIBRARY(matador-sql STATIC ${SOURCES} ${HEADER})

TARGET_LINK_LIBRARIES(matador-sql matador-logger matador-utils)

# Set the build version (VERSION) and the API version (SOVERSION)
SET_TARGET_PROPERTIES(matador-sql
  PROPERTIES
  VERSION ${APP_VERSION}
  SOVERSION ${APP_MAJOR_VERSION})

SOURCE_GROUP("include\\matador\\sql" FILES ${HEADER})
SOURCE_GROUP("src\\matador\\sql" FILES ${SOURCES})

INSTALL(
  TARGETS matador-sql
  ARCHIVE  DESTINATION lib
  LIBRARY DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  TARGETS matador-sql
  DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  FILES ${HEADER}
  DESTINATION include/matador/sql
  COMPONENT headers
)
