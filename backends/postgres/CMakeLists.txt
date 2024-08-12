set(HEADER
  include/postgres_connection.hpp
  include/postgres_error.hpp
  include/postgres_result_reader.hpp
  include/postgres_dialect.hpp
  include/postgres_statement.hpp
  include/postgres_parameter_binder.h
)

set(SOURCES
  src/postgres_connection.cpp
  src/postgres_error.cpp
  src/postgres_result_reader.cpp
  src/postgres_dialect.cpp
  src/postgres_statement.cpp
  src/postgres_parameter_binder.cpp
)

set(LIBRARY_TARGET matador-postgres)

add_subdirectory(test)

add_library(${LIBRARY_TARGET} MODULE ${SOURCES} ${HEADER})

set_target_properties(${LIBRARY_TARGET}
  PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/backends"
)

target_include_directories(${LIBRARY_TARGET} PRIVATE
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/backends/postgres/include
  ${PostgreSQL_INCLUDE_DIRS})

target_link_libraries(${LIBRARY_TARGET}
    matador-utils
    matador-query
    ${PostgreSQL_LIBRARIES})
