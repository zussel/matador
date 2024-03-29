SET(SOURCES
  postgresql_connection.cpp
  postgresql_dialect.cpp
  postgresql_exception.cpp
  postgresql_result.cpp
  postgresql_statement.cpp
  postgresql_getvalue.cpp
  postgresql_prepared_result.cpp
  postgresql_dialect_compiler.cpp
  postgresql_parameter_binder.cpp)

SET(HEADER
  ../../../include/matador/db/postgresql/postgresql_connection.hpp
  ../../../include/matador/db/postgresql/postgresql_dialect.hpp
  ../../../include/matador/db/postgresql/postgresql_exception.hpp
  ../../../include/matador/db/postgresql/postgresql_result.hpp
  ../../../include/matador/db/postgresql/postgresql_statement.hpp
  ../../../include/matador/db/postgresql/postgresql_getvalue.hpp
  ../../../include/matador/db/postgresql/postgresql_prepared_result.hpp
  ../../../include/matador/db/postgresql/postgresql_dialect_compiler.hpp
  ../../../include/matador/db/postgresql/postgresql_parameter_binder.hpp)

ADD_LIBRARY(matador-postgresql SHARED ${SOURCES} ${HEADER})

TARGET_LINK_LIBRARIES(matador-postgresql matador-utils matador-sql ${PostgreSQL_LIBRARIES})

# Set the build version (VERSION) and the API version (SOVERSION)
SET_TARGET_PROPERTIES(matador-postgresql
  PROPERTIES
  VERSION ${APP_VERSION}
  SOVERSION ${APP_MAJOR_VERSION})

SOURCE_GROUP("include\\matador\\db\\postgresql" FILES ${HEADER})
SOURCE_GROUP("src\\matador\\db\\postgresql" FILES ${SOURCES})

INSTALL(
  TARGETS matador-postgresql
  ARCHIVE	DESTINATION lib
  LIBRARY DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  TARGETS matador-postgresql
  DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  FILES ${HEADER}
  DESTINATION include/matador/db/postgresql
  COMPONENT headers
)
