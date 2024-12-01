set(HEADER
  include/odbc_connection.hpp
  include/odbc_error.hpp
  include/odbc_dialect.hpp
  include/odbc_result_reader.hpp
  include/odbc_statement.hpp
  include/odbc_parameter_binder.hpp
  include/odbc_handle.hpp
  include/odbc_result_binder.hpp
)

set(SOURCES
  src/odbc_connection.cpp
  src/odbc_error.cpp
  src/odbc_dialect.cpp
  src/odbc_result_reader.cpp
  src/odbc_statement.cpp
  src/odbc_parameter_binder.cpp
  src/odbc_handle.cpp
  src/odbc_result_binder.cpp
)

set(LIBRARY_TARGET matador-odbc)

add_subdirectory(test)

add_library(${LIBRARY_TARGET} MODULE ${SOURCES} ${HEADER})

set_target_properties(${LIBRARY_TARGET}
  PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/backends"
)

target_include_directories(${LIBRARY_TARGET} PRIVATE
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/backends/odbc/include
  ${ODBC_INCLUDE_DIRS})

target_link_libraries(${LIBRARY_TARGET}
    matador-utils
    matador-query
    ${ODBC_LIBRARIES})

add_executable(odbc_test odbc_test.cpp)

target_include_directories(odbc_test PRIVATE
  ${ODBC_INCLUDE_DIRS}
)

target_link_libraries(odbc_test
  ${ODBC_LIBRARIES}
)
