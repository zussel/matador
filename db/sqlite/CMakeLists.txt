SET(SQLITE_DATABASE_SOURCES
		src/sqlite_connection.cpp
  src/sqlite_exception.cpp
  src/sqlite_statement.cpp
  src/sqlite_result.cpp
  src/sqlite_prepared_result.cpp
	src/sqlite_dialect.cpp)

SET(SQLITE_DATABASE_HEADER
		include/sqlite_connection.hpp
  include/sqlite_exception.hpp
  include/sqlite_statement.hpp
  include/sqlite_result.hpp
  include/sqlite_prepared_result.hpp
  include/sqlite_types.hpp
	include/sqlite_dialect.hpp)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/db/sqlite/include)

ADD_LIBRARY(oos-sqlite SHARED
  ${SQLITE_DATABASE_SOURCES}
  ${SQLITE_DATABASE_HEADER}
)

SET_TARGET_PROPERTIES(oos-sqlite
                      PROPERTIES
                      VERSION 0.2.1
                      SOVERSION 0)

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
