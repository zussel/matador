SET(MYSQL_DATABASE_SOURCES
  src/mysql_database.cpp
  src/mysql_exception.cpp
  src/mysql_statement.cpp
  src/mysql_result.cpp
  src/mysql_prepared_result.cpp
  src/mysql_column_fetcher.cpp
)

SET(MYSQL_DATABASE_HEADER
  include/mysql_database.hpp
  include/mysql_exception.hpp
  include/mysql_statement.hpp
  include/mysql_result.hpp
  include/mysql_prepared_result.hpp
  include/mysql_types.hpp
  include/mysql_column_fetcher.hpp
)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/db/mysql/include)

ADD_LIBRARY(oos-mysql SHARED
  ${MYSQL_DATABASE_SOURCES}
  ${MYSQL_DATABASE_HEADER}
)

SET_TARGET_PROPERTIES(oos-mysql
                      PROPERTIES
                      VERSION 0.2.1
                      SOVERSION 0)

TARGET_LINK_LIBRARIES(oos-mysql oos ${MYSQL_LIBRARY})

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
