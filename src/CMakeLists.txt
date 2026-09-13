configure_file (${CMAKE_SOURCE_DIR}/include/matador/version.hpp.in ${CMAKE_BINARY_DIR}/version.hpp)

#ADD_SUBDIRECTORY(query)
#ADD_SUBDIRECTORY(utils)

add_library(matador
    ../include/matador/query/basic_schema.hpp
    ../include/matador/query/basic_table_info.hpp
    ../include/matador/query/query_data.hpp
    ../include/matador/query/relation_endpoint.hpp
    ../include/matador/query/relation_type.hpp
    ../include/matador/query/resolver_service.hpp
    ../include/matador/query/schema.hpp
    ../include/matador/utils/flags.hpp
    query/basic_schema.cpp
    query/basic_table_info.cpp
    query/column.cpp
    query/column_options.cpp
    query/column_value.cpp
    query/constraint.cpp
    query/criteria/abstract_column_criteria.cpp
    query/criteria/between_criteria.cpp
    query/criteria/binary_criteria.cpp
    query/criteria/check_null_criteria.cpp
    query/criteria/collection_criteria.cpp
    query/criteria/criteria_operators.cpp
    query/criteria/like_criteria.cpp
    query/criteria/logical_criteria.cpp
    query/criteria/not_criteria.cpp
    query/criteria_evaluator.cpp
    query/dialect.cpp
    query/error_code.cpp
    query/expression/binary_column_expression.cpp
    query/expression/column_expression.cpp
    query/expression/expression_operators.cpp
    query/expression/placeholder_expression.cpp
    query/expression/table_column_expression.cpp
    query/expression/value_expression.cpp
    query/expression_evaluator.cpp
    query/field.cpp
    query/foreign_options.cpp
    query/identifier.cpp
    query/intermediates/alter_intermediate.cpp
    query/intermediates/alter_table_intermediate.cpp
    query/intermediates/create_intermediate.cpp
    query/intermediates/executable_query.cpp
    query/intermediates/from_intermediate.cpp
    query/intermediates/group_by_intermediate.cpp
    query/intermediates/fetchable_query.cpp
    query/intermediates/insert_intermediate.cpp
    query/intermediates/intermediate.cpp
    query/intermediates/into_intermediate.cpp
    query/intermediates/join_intermediate.cpp
    query/intermediates/limit_intermediate.cpp
    query/intermediates/offset_intermediate.cpp
    query/intermediates/order_by_intermediate.cpp
    query/intermediates/order_direction_intermediate.cpp
    query/intermediates/select_intermediate.cpp
    query/intermediates/values_intermediate.cpp
    query/intermediates/where_intermediate.cpp
    query/internal/attribute_string_writer.cpp
    query/internal/basic_type_to_string_visitor.cpp
    query/internal/collection_utils.cpp
    query/internal/column_value_pair.cpp
    query/internal/identifier_discover_service.cpp
    query/internal/query_parts.cpp
    query/internal/query_result_impl.cpp
    query/internal/result_binder.cpp
    query/internal/string_builder_utils.cpp
    query/internal/value_extractor.cpp
    query/observer.cpp
    query/primary_key_options.cpp
    query/query.cpp
    query/query_builder.cpp
    query/query_part.cpp
    query/query_utils.cpp
    query/record.cpp
    query/relation_endpoint.cpp
    query/schema_node.cpp
    query/schema_node_iterator.cpp
    query/table.cpp
    query/table.cpp
    query/table_generator.cpp
    utils/error.cpp
    utils/error_exception.cpp
    utils/errors.cpp
    utils/default_type_traits.cpp
    utils/os.cpp
    utils/string.cpp
    utils/types.cpp
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