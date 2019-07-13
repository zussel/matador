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
  ${CMAKE_SOURCE_DIR}/include/matador/db/sqlite/sqlite_connection.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/db/sqlite/sqlite_exception.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/db/sqlite/sqlite_statement.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/db/sqlite/sqlite_result.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/db/sqlite/sqlite_prepared_result.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/db/sqlite/sqlite_types.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/db/sqlite/sqlite_dialect.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/db/sqlite/sqlite_dialect_compiler.hpp
)

ADD_LIBRARY(matador-sqlite SHARED ${SOURCES} ${HEADER})

TARGET_LINK_LIBRARIES(matador-sqlite matador-utils matador-sql ${SQLITE3_LIBRARY})

# Set the build version (VERSION) and the API version (SOVERSION)
SET_TARGET_PROPERTIES(matador-sqlite
  PROPERTIES
  VERSION ${APP_VERSION}
  SOVERSION ${APP_MAJOR_VERSION})

SOURCE_GROUP("include\\matador\\db\\sqlite" FILES ${HEADER})
SOURCE_GROUP("src\\matador\\db\\sqlite" FILES ${SOURCES})

INSTALL(
  TARGETS matador-sqlite
  ARCHIVE	DESTINATION lib
  LIBRARY DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  TARGETS matador-sqlite
  DESTINATION lib
  COMPONENT libraries
)
