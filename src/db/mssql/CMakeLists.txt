SET(SOURCES
  mssql_connection.cpp
  mssql_exception.cpp
  mssql_statement.cpp
  mssql_result.cpp
  mssql_dialect.cpp
  mssql_dialect_compiler.cpp
  mssql_dialect_linker.cpp
  mssql_parameter_binder.cpp)

SET(HEADER
  ../../../include/matador/db/mssql/mssql_connection.hpp
  ../../../include/matador/db/mssql/mssql_exception.hpp
  ../../../include/matador/db/mssql/mssql_statement.hpp
  ../../../include/matador/db/mssql/mssql_result.hpp
  ../../../include/matador/db/mssql/mssql_types.hpp
  ../../../include/matador/db/mssql/mssql_dialect.hpp
  ../../../include/matador/db/mssql/mssql_dialect_compiler.hpp
  ../../../include/matador/db/mssql/mssql_dialect_linker.hpp
  ../../../include/matador/db/mssql/mssql_parameter_binder.hpp)

ADD_LIBRARY(matador-mssql SHARED ${SOURCES} ${HEADER})

TARGET_LINK_LIBRARIES(matador-mssql matador-utils matador-sql ${ODBC_LIBRARY})

# Set the build version (VERSION) and the API version (SOVERSION)
SET_TARGET_PROPERTIES(matador-mssql
  PROPERTIES
  VERSION ${APP_VERSION}
  SOVERSION ${APP_MAJOR_VERSION})

SOURCE_GROUP("include\\matador\\db\\mssql" FILES ${HEADER})
SOURCE_GROUP("src\\matador\\db\\mssql" FILES ${SOURCES})

INSTALL(
  TARGETS matador-mssql
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  TARGETS matador-mssql
  DESTINATION lib
  COMPONENT libraries
)

INSTALL(
    FILES ${HEADER}
    DESTINATION include/matador/db/mssql
    COMPONENT headers
)
