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
)

SET(HEADER
  ${CMAKE_SOURCE_DIR}/include/oos/sql/condition.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/sql/connection.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/sql/connection_factory.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/sql/connection_impl.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/sql/result.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/sql/result_impl.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/sql/memory_connection.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/sql/query.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/sql/sql.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/sql/row.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/sql/value.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/sql/statement.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/sql/statement_impl.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/sql/types.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/sql/token.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/sql/sql_exception.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/sql/field.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/sql/basic_dialect.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/sql/typed_column_serializer.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/sql/column.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/sql/dialect_token.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/sql/column_serializer.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/sql/value_serializer.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/sql/value_column_serializer.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/sql/commands.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/sql/basic_query.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/sql/token_list.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/sql/token_visitor.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/sql/basic_dialect_compiler.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/sql/basic_dialect_linker.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/sql/query_value_column_processor.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/sql/query_value_creator.hpp)

ADD_LIBRARY(oos-sql SHARED ${SOURCES} ${HEADER})

# Set the build version (VERSION) and the API version (SOVERSION)
SET_TARGET_PROPERTIES(oos-sql
  PROPERTIES
  VERSION 0.5.0
  SOVERSION 0)

SOURCE_GROUP("include\\oos\\sql" FILES ${HEADER})
SOURCE_GROUP("src\\oos\\sql" FILES ${SOURCES})

INSTALL(
  TARGETS oos-sql
  ARCHIVE  DESTINATION lib
  LIBRARY DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  TARGETS oos-sql
  DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  FILES ${HEADER}
  DESTINATION include/oos/sql
  COMPONENT headers
)
