configure_file (${CMAKE_SOURCE_DIR}/include/matador/version.hpp.in ${CMAKE_BINARY_DIR}/version.hpp)

#ADD_SUBDIRECTORY(query)
#ADD_SUBDIRECTORY(utils)

add_library(matador
    utils/default_type_traits.cpp
    utils/error.cpp
    utils/errors.cpp
    utils/field_attributes.cpp
    utils/identifier.cpp
    utils/os.cpp
    utils/string.cpp
    utils/types.cpp
    utils/value.cpp
    utils/version.cpp
)

include(GenerateExportHeader)

generate_export_header(matador)

if(ENABLE_CLANG_TIDY AND CLANG_TIDY_EXE)
    set_target_properties(matador PROPERTIES
            CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
endif()

target_include_directories(matador
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:include>
)

# Compiler warnings based on target platform
if(MSVC)
    target_compile_options(matador PRIVATE /W4 /permissive-)
else()
    target_compile_options(matador PRIVATE -Wall -Wextra -Wpedantic -Wconversion)
endif()

# Install & Export Configuration
include(GNUInstallDirs)

install(TARGETS matador
        EXPORT MatadorTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT MatadorTargets
        FILE MatadorTargets.cmake
        NAMESPACE matador::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/matador
)