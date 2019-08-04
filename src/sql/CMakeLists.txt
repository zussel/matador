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
    connection_impl.cpp)

SET(HEADER
  ${CMAKE_SOURCE_DIR}/include/matador/sql/condition.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/sql/connection.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/sql/connection_factory.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/sql/connection_impl.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/sql/result.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/sql/result_impl.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/sql/memory_connection.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/sql/query.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/sql/sql.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/sql/row.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/sql/value.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/sql/statement.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/sql/statement_impl.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/sql/types.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/sql/token.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/sql/sql_exception.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/sql/field.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/sql/basic_dialect.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/sql/typed_column_serializer.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/sql/column.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/sql/dialect_token.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/sql/column_serializer.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/sql/value_serializer.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/sql/value_column_serializer.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/sql/commands.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/sql/basic_query.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/sql/token_list.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/sql/token_visitor.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/sql/basic_dialect_compiler.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/sql/basic_dialect_linker.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/sql/query_value_column_processor.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/sql/query_value_creator.hpp)

ADD_LIBRARY(matador-sql SHARED ${SOURCES} ${HEADER})

TARGET_LINK_LIBRARIES(matador-sql matador-utils)

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
