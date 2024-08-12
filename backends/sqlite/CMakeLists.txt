set(HEADER
  include/sqlite_connection.hpp
  include/sqlite_error.hpp
  include/sqlite_dialect.hpp
  include/sqlite_result_reader.hpp
  include/sqlite_statement.hpp
  include/sqlite_parameter_binder.h
  include/sqlite_prepared_result_reader.hpp
)

set(SOURCES
  src/sqlite_connection.cpp
  src/sqlite_error.cpp
  src/sqlite_dialect.cpp
  src/sqlite_result_reader.cpp
  src/sqlite_statement.cpp
  src/sqlite_parameter_binder.cpp
  src/sqlite_prepared_result_reader.cpp
)

set(LIBRARY_TARGET matador-sqlite)

add_subdirectory(test)

add_library(${LIBRARY_TARGET} MODULE ${SOURCES} ${HEADER})

set_target_properties(${LIBRARY_TARGET}
  PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/backends"
)

target_include_directories(${LIBRARY_TARGET} PRIVATE
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/backends/sqlite/include
  ${SQLite3_INCLUDE_DIRS})

target_link_libraries(${LIBRARY_TARGET}
    matador-utils
    matador-query
    ${SQLite3_LIBRARIES})
