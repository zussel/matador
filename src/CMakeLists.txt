CONFIGURE_FILE (${CMAKE_SOURCE_DIR}/include/matador/version.hpp.in ${CMAKE_BINARY_DIR}/version.hpp)

ADD_SUBDIRECTORY(object)
ADD_SUBDIRECTORY(orm)
ADD_SUBDIRECTORY(sql)
ADD_SUBDIRECTORY(utils)
ADD_SUBDIRECTORY(json)
ADD_SUBDIRECTORY(logger)
ADD_SUBDIRECTORY(net)
ADD_SUBDIRECTORY(http)
ADD_SUBDIRECTORY(unit)

ADD_SUBDIRECTORY(db)

ADD_CUSTOM_TARGET(matador
  COMMAND ${CMAKE_COMMAND} -E echo Building matador libs
  DEPENDS
  matador-unit
  matador-utils
  matador-json
  matador-logger
  matador-net
  matador-http
  matador-object
  matador-sql
  matador-orm
)

IF (SQLITE3_FOUND)
  ADD_DEPENDENCIES(matador matador-sqlite)
ENDIF(SQLITE3_FOUND)

IF (MYSQL_FOUND)
  ADD_DEPENDENCIES(matador matador-mysql)
ENDIF(MYSQL_FOUND)

IF (ODBC_FOUND)
  ADD_DEPENDENCIES(matador matador-mssql)
ENDIF(ODBC_FOUND)

IF (POSTGRESQL_FOUND)
  ADD_DEPENDENCIES(matador matador-postgresql)
ENDIF(POSTGRESQL_FOUND)

INSTALL(
  FILES ${CMAKE_BINARY_DIR}/version.hpp
  DESTINATION include/matador
  COMPONENT headers
)
