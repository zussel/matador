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
  models/product.hpp
  models/order.hpp
  models/order_details.hpp
  models/category.hpp
  models/supplier.hpp
  models/airplane.hpp
  models/flight.hpp
  models/person.hpp
  models/coordinate.hpp
  models/location.hpp
  models/optional.hpp
  models/author.hpp
  models/book.hpp
  models/recipe.hpp
  models/student.hpp
  tools/auto_reset_event.cpp
  tools/auto_reset_event.hpp
  sql/AnyTypeToVisitorTest.cpp
  sql/ConvertTest.cpp
  sql/EntityQueryBuilderTest.cpp
  sql/ColumnTest.cpp
  sql/QueryBuilderTest.cpp
  sql/FieldTest.cpp
  sql/TableDefinitionTest.cpp
  sql/ConnectionPoolTest.cpp
  sql/BackendProviderTest.cpp
  sql/ColumnDefinitionGeneratorTest.cpp
  sql/ColumnGeneratorTest.cpp
  sql/ValueGeneratorTest.cpp
  sql/ValueTest.cpp
  utils/ConvertTest.cpp
  utils/ResultTest.cpp
#  sql/ColumnValueVisitorTest.cpp
#  sql/QueryValueColumnProcessorTest.cpp
  object/ObserverListCopyCreatorTest.cpp
  object/ObjectPrototypeTest.cpp
  object/ObjectStoreAttachTest.cpp
  object/ObjectStoreRelationTest.cpp
  object/ObjectStoreAttachTestModels.hpp
  sql/ConditionTests.cpp
)

target_link_libraries(tests PRIVATE
        Catch2::Catch2WithMain
        matador-utils
        matador-object
        matador-query
        ${CMAKE_DL_LIBS})
target_include_directories(tests PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/include PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/test/catch2)

catch_discover_tests(tests)
