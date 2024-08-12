set(HEADER
  include/mysql_connection.hpp
  include/mysql_error.hpp
  include/mysql_result_reader.hpp
  include/mysql_dialect.hpp
  include/mysql_statement.hpp
  include/mysql_parameter_binder.hpp
  include/mysql_prepared_result_reader.hpp
)

set(SOURCES
  src/mysql_connection.cpp
  src/mysql_error.cpp
  src/mysql_result_reader.cpp
  src/mysql_dialect.cpp
  src/mysql_statement.cpp
  src/mysql_parameter_binder.cpp
  src/mysql_prepared_result_reader.cpp
)

set(LIBRARY_TARGET matador-mysql)

add_subdirectory(test)

add_library(${LIBRARY_TARGET} MODULE ${SOURCES} ${HEADER})

set_target_properties(${LIBRARY_TARGET}
  PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/backends"
)

target_include_directories(${LIBRARY_TARGET} PRIVATE
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/backends/mysql/include
  ${MYSQL_INCLUDE_DIR})

target_link_libraries(${LIBRARY_TARGET}
    matador-utils
    matador-query
    ${MYSQL_LIBRARY})
