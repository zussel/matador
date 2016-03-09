SET(MSSQL_DATABASE_SOURCES
  src/mssql_connection.cpp
  src/mssql_exception.cpp
  src/mssql_statement.cpp
  src/mssql_result.cpp
  src/mssql_dialect.cpp
)

SET(MSSQL_DATABASE_HEADER
  include/mssql_connection.hpp
  include/mssql_exception.hpp
  include/mssql_statement.hpp
  include/mssql_result.hpp
  include/mssql_types.hpp
  include/mssql_dialect.hpp
)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/db/mssql/include)

ADD_LIBRARY(oos-mssql SHARED
  ${MSSQL_DATABASE_SOURCES}
  ${MSSQL_DATABASE_HEADER}
)

SET_TARGET_PROPERTIES(oos-mssql
                      PROPERTIES
                      VERSION 0.2.1
                      SOVERSION 0)

TARGET_LINK_LIBRARIES(oos-mssql oos ${ODBC_LIBRARY})

INSTALL(
	TARGETS oos-mssql
	ARCHIVE	DESTINATION lib
	LIBRARY DESTINATION lib
	COMPONENT libraries
)

INSTALL(
	TARGETS oos-mssql
	DESTINATION lib
	COMPONENT libraries
)
