SET(SOURCES
  mysql_connection.cpp
  mysql_exception.cpp
  mysql_statement.cpp
  mysql_result.cpp
  mysql_prepared_result.cpp
  mysql_dialect.cpp
)

SET(HEADER
  ${CMAKE_SOURCE_DIR}/include/matador/db/mysql/mysql_connection.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/db/mysql/mysql_exception.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/db/mysql/mysql_statement.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/db/mysql/mysql_result.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/db/mysql/mysql_prepared_result.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/db/mysql/mysql_result_info.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/db/mysql/mysql_types.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/db/mysql/mysql_dialect.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/db/mysql/mysql_constants.hpp
)

SET(HEADER_INSTALL
  ${CMAKE_SOURCE_DIR}/include/matador/db/mysql/mysql_constants.hpp
)

ADD_LIBRARY(matador-mysql SHARED ${SOURCES} ${HEADER})

TARGET_LINK_LIBRARIES(matador-mysql matador-utils matador-sql ${MYSQL_LIBRARY})

# Set the build version (VERSION) and the API version (SOVERSION)
SET_TARGET_PROPERTIES(matador-mysql
  PROPERTIES
  VERSION 0.5.0
  SOVERSION 0)

SOURCE_GROUP("include\\matador\\db\\mysql" FILES ${HEADER})
SOURCE_GROUP("src\\matador\\db\\mysql" FILES ${SOURCES})

INSTALL(
  TARGETS matador-mysql
  ARCHIVE	DESTINATION lib
  LIBRARY DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  TARGETS matador-mysql
  DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  FILES ${HEADER_INSTALL}
  DESTINATION include/matador/db/mysql
  COMPONENT headers
)
