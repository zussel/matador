# Download Catch2 v3 via CPM
CPMAddPackage(
        NAME Catch2
        GITHUB_REPOSITORY catchorg/Catch2
        VERSION 3.16.0
)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

add_executable(MatadorTests
    query/ColumnTest.cpp
    query/ColumnExpressionTest.cpp
    query/IdentifierTest.cpp
    utils/ConvertTest.cpp
    utils/ResultTest.cpp
    utils/StringTest.cpp
    utils/VersionTest.cpp
)

target_include_directories(MatadorTests
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>/../src
)

set_target_properties(MatadorTests PROPERTIES
        CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")

target_link_libraries(MatadorTests PRIVATE matador Catch2::Catch2WithMain)

if(ENABLE_CODE_COVERAGE)
    setup_target_for_coverage_gcovr_xml(
            NAME coverage
            EXECUTABLE unit_tests
            DEPENDENCIES unit_tests
    )
endif()