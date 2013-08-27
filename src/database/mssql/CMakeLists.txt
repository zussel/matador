SET(MSSQL_DATABASE_SOURCES
  mssql_database.cpp
  mssql_exception.cpp
  mssql_statement.cpp
  mssql_result.cpp
  mssql_prepared_result.cpp
)

SET(MSSQL_DATABASE_HEADER
  ${PROJECT_SOURCE_DIR}/include/database/mssql/mssql_database.hpp
  ${PROJECT_SOURCE_DIR}/include/database/mssql/mssql_exception.hpp
  ${PROJECT_SOURCE_DIR}/include/database/mssql/mssql_statement.hpp
  ${PROJECT_SOURCE_DIR}/include/database/mssql/mssql_result.hpp
  ${PROJECT_SOURCE_DIR}/include/database/mssql/mssql_prepared_result.hpp
  ${PROJECT_SOURCE_DIR}/include/database/mssql/mssql_types.hpp
)

ADD_LIBRARY(oos-mssql SHARED
  ${MSSQL_DATABASE_SOURCES}
)

SET_TARGET_PROPERTIES(oos-mssql
                      PROPERTIES
                      VERSION 0.1.0
                      SOVERSION 1)

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
