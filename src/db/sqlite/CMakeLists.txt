SET(SOURCES
  sqlite_connection.cpp
  sqlite_exception.cpp
  sqlite_statement.cpp
  sqlite_result.cpp
  sqlite_prepared_result.cpp
  sqlite_dialect.cpp
  sqlite_dialect_compiler.cpp
)

SET(HEADER
  ${CMAKE_SOURCE_DIR}/include/oos/db/sqlite/sqlite_connection.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/db/sqlite/sqlite_exception.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/db/sqlite/sqlite_statement.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/db/sqlite/sqlite_result.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/db/sqlite/sqlite_prepared_result.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/db/sqlite/sqlite_types.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/db/sqlite/sqlite_dialect.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/db/sqlite/sqlite_dialect_compiler.hpp
)

ADD_LIBRARY(oos-sqlite SHARED ${SOURCES} ${HEADER})

TARGET_LINK_LIBRARIES(oos-sqlite oos-utils oos-sql ${SQLITE3_LIBRARY})

# Set the build version (VERSION) and the API version (SOVERSION)
SET_TARGET_PROPERTIES(oos-sqlite
  PROPERTIES
  VERSION 0.5.0
  SOVERSION 0)

SOURCE_GROUP("include\\oos\\db\\sqlite" FILES ${HEADER})
SOURCE_GROUP("src\\oos\\db\\sqlite" FILES ${SOURCES})

INSTALL(
  TARGETS oos-sqlite
  ARCHIVE	DESTINATION lib
  LIBRARY DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  TARGETS oos-sqlite
  DESTINATION lib
  COMPONENT libraries
)
