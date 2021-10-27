SET(SOURCES
  mysql_connection.cpp
  mysql_exception.cpp
  mysql_statement.cpp
  mysql_result.cpp
  mysql_prepared_result.cpp
  mysql_dialect.cpp
  mysql_parameter_binder.cpp)

SET(HEADER
  ../../../include/matador/db/mysql/mysql_constants.hpp
  ../../../include/matador/db/mysql/mysql_connection.hpp
  ../../../include/matador/db/mysql/mysql_exception.hpp
  ../../../include/matador/db/mysql/mysql_statement.hpp
  ../../../include/matador/db/mysql/mysql_result.hpp
  ../../../include/matador/db/mysql/mysql_prepared_result.hpp
  ../../../include/matador/db/mysql/mysql_result_info.hpp
  ../../../include/matador/db/mysql/mysql_types.hpp
  ../../../include/matador/db/mysql/mysql_dialect.hpp
  ../../../include/matador/db/mysql/mysql_constants.hpp
  ../../../include/matador/db/mysql/mysql_parameter_binder.hpp ../../../include/matador/db/mysql/mysql_bool.hpp)

ADD_LIBRARY(matador-mysql SHARED ${SOURCES} ${HEADER})

IF (MSVC)
    target_compile_definitions(matador-mysql PUBLIC WIN32_LEAN_AND_MEAN)
ENDIF()


TARGET_LINK_LIBRARIES(matador-mysql matador-utils matador-sql ${MYSQL_LIBRARY})

# Set the build version (VERSION) and the API version (SOVERSION)
SET_TARGET_PROPERTIES(matador-mysql
  PROPERTIES
  VERSION ${APP_VERSION}
  SOVERSION ${APP_MAJOR_VERSION})

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
  FILES ${HEADER}
  DESTINATION include/matador/db/mysql
  COMPONENT headers
)
