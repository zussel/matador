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
  sql/ValueTest.cpp
  utils/ConvertTest.cpp
  sql/ColumnValueVisitorTest.cpp
  sql/QueryValueColumnProcessorTest.cpp
  object/ObserverListCopyCreatorTest.cpp
  object/ObjectPrototypeTest.cpp
  object/ObjectStoreAttachTest.cpp
  object/ObjectStoreRelationTest.cpp
  object/ObjectStoreAttachTestModels.hpp
)

target_link_libraries(tests PRIVATE
        Catch2::Catch2WithMain
        matador-utils
        matador-object
        matador-sql
        ${CMAKE_DL_LIBS})
target_include_directories(tests PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/include)

catch_discover_tests(tests)
