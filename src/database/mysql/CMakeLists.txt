SET(MYSQL_DATABASE_SOURCES
  mysql_database.cpp
  mysql_exception.cpp
  mysql_statement.cpp
  mysql_sequencer.cpp
  mysql_table.cpp
  mysql_result.cpp
  mysql_prepared_result.cpp
)

SET(MYSQL_DATABASE_HEADER
  ${PROJECT_SOURCE_DIR}/include/database/mysql/mysql_database.hpp
  ${PROJECT_SOURCE_DIR}/include/database/mysql/mysql_exception.hpp
  ${PROJECT_SOURCE_DIR}/include/database/mysql/mysql_statement.hpp
  ${PROJECT_SOURCE_DIR}/include/database/mysql/mysql_sequencer.hpp
  ${PROJECT_SOURCE_DIR}/include/database/mysql/mysql_table.hpp
  ${PROJECT_SOURCE_DIR}/include/database/mysql/mysql_result.hpp
  ${PROJECT_SOURCE_DIR}/include/database/mysql/mysql_prepared_result.hpp
  ${PROJECT_SOURCE_DIR}/include/database/mysql/mysql_types.hpp
)

ADD_LIBRARY(oos-mysql SHARED
  ${MYSQL_DATABASE_SOURCES}
)

SET_TARGET_PROPERTIES(oos-mysql
                      PROPERTIES
                      VERSION 0.1.0
                      SOVERSION 1)

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
