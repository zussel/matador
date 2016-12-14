SET(SOURCES
  mssql_connection.cpp
  mssql_exception.cpp
  mssql_statement.cpp
  mssql_result.cpp
  mssql_dialect.cpp
  mssql_dialect_compiler.cpp
  mssql_dialect_linker.cpp
)

SET(HEADER
  ${CMAKE_SOURCE_DIR}/include/oos/db/mssql/mssql_connection.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/db/mssql/mssql_exception.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/db/mssql/mssql_statement.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/db/mssql/mssql_result.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/db/mssql/mssql_types.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/db/mssql/mssql_dialect.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/db/mssql/mssql_dialect_compiler.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/db/mssql/mssql_dialect_linker.hpp
)

ADD_LIBRARY(oos-mssql SHARED ${SOURCES} ${HEADER})

# Set the build version (VERSION) and the API version (SOVERSION)
SET_TARGET_PROPERTIES(oos-mssql
  PROPERTIES
  VERSION 0.5.0
  SOVERSION 0)

SOURCE_GROUP("include\\oos\\db\\mssql" FILES ${HEADER})
SOURCE_GROUP("src\\oos\\db\\mssql" FILES ${SOURCES})

INSTALL(
  TARGETS oos-mssql
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  TARGETS oos-mssql
  DESTINATION lib
  COMPONENT libraries
)
