SET(SOURCES
  mysql_connection.cpp
  mysql_exception.cpp
  mysql_statement.cpp
  mysql_result.cpp
  mysql_prepared_result.cpp
  mysql_dialect.cpp
)

SET(HEADER
  ${CMAKE_SOURCE_DIR}/include/oos/db/mysql/mysql_connection.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/db/mysql/mysql_exception.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/db/mysql/mysql_statement.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/db/mysql/mysql_result.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/db/mysql/mysql_prepared_result.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/db/mysql/mysql_result_info.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/db/mysql/mysql_types.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/db/mysql/mysql_dialect.hpp
)

ADD_LIBRARY(oos-mysql SHARED ${SOURCES} ${HEADER})

TARGET_LINK_LIBRARIES(oos-mysql oos-utils oos-sql ${MYSQL_LIBRARY})

# Set the build version (VERSION) and the API version (SOVERSION)
SET_TARGET_PROPERTIES(oos-mysql
  PROPERTIES
  VERSION 0.5.0
  SOVERSION 0)

SOURCE_GROUP("include\\oos\\db\\mysql" FILES ${HEADER})
SOURCE_GROUP("src\\oos\\db\\mysql" FILES ${SOURCES})

INSTALL(
  TARGETS oos-mysql
  ARCHIVE	DESTINATION lib
  LIBRARY DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  TARGETS oos-mysql
  DESTINATION lib
  COMPONENT libraries
)
