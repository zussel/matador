SET(SQLITE_DATABASE_SOURCES
  sqlite_database.cpp
  sqlite_exception.cpp
  sqlite_statement.cpp
  sqlite_sequencer.cpp
  sqlite_table.cpp
  sqlite_result.cpp
)

SET(SQLITE_DATABASE_HEADER
  ${PROJECT_SOURCE_DIR}/include/database/sqlite/sqlite_database.hpp
  ${PROJECT_SOURCE_DIR}/include/database/sqlite/sqlite_exception.hpp
  ${PROJECT_SOURCE_DIR}/include/database/sqlite/sqlite_statement.hpp
  ${PROJECT_SOURCE_DIR}/include/database/sqlite/sqlite_sequencer.hpp
  ${PROJECT_SOURCE_DIR}/include/database/sqlite/sqlite_result.hpp
  ${PROJECT_SOURCE_DIR}/include/database/sqlite/sqlite_table.hpp
  ${PROJECT_SOURCE_DIR}/include/database/sqlite/sqlite_types.hpp
)

ADD_LIBRARY(oos-sqlite SHARED
  ${SQLITE_DATABASE_SOURCES}
)

SET_TARGET_PROPERTIES(oos-sqlite
                      PROPERTIES
                      VERSION 0.1.0
                      SOVERSION 1)

TARGET_LINK_LIBRARIES(oos-sqlite oos ${SQLITE3_LIBRARY})

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
