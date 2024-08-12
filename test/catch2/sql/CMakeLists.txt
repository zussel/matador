Include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.5.4 # or a later release
)

FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)

add_executable(tests
  QueryBuilderTest.cpp
  TableDefinitionTest.cpp
  ConnectionPoolTest.cpp
  BackendProviderTest.cpp
  ../models/product.hpp
  ../models/order.hpp
  ../models/order_details.hpp
  ColumnDefinitionGeneratorTest.cpp
  ColumnGeneratorTest.cpp
  ValueGeneratorTest.cpp
  ../models/category.hpp
  ../models/supplier.hpp
  ../models/airplane.hpp
  ../models/flight.hpp
  ../models/person.hpp
  AnyTypeToVisitorTest.cpp
  ColumnTest.cpp
  ../models/coordinate.hpp
  ../models/location.hpp
  ../models/optional.hpp
  ConvertTest.cpp
  EntityQueryBuilderTest.cpp
  ../models/author.hpp
  ../models/book.hpp
  FieldTest.cpp
  ../models/recipe.hpp
  ValueTest.cpp
    ../utils/ResultTest.cpp
  utils/auto_reset_event.hpp
  utils/auto_reset_event.cpp
  ../models/student.hpp)

target_link_libraries(tests PRIVATE
  Catch2::Catch2WithMain
  matador
  ${CMAKE_DL_LIBS}
  ${SQLite3_LIBRARIES}
  ${PostgreSQL_LIBRARY})
target_include_directories(tests PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/include)

catch_discover_tests(tests)
