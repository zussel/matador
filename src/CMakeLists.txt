configure_file (${CMAKE_SOURCE_DIR}/include/matador/version.hpp.in ${CMAKE_BINARY_DIR}/version.hpp)

#ADD_SUBDIRECTORY(query)
#ADD_SUBDIRECTORY(utils)

add_library(matador
    ../include/matador/query/builder/query_data.hpp
    ../include/matador/query/model/relation_endpoint.hpp
    ../include/matador/query/model/relation_type.hpp
    ../include/matador/query/resolver_service.hpp
    ../include/matador/query/schema/schema.hpp
    ../include/matador/query/schema/basic_schema.hpp
    ../include/matador/query/schema/basic_table_info.hpp
    ../include/matador/utils/flags.hpp
    query/builder/query.cpp
    query/builder/query_builder.cpp
    query/builder/query_part.cpp
    query/builder/query_utils.cpp
    query/criteria/abstract_column_criteria.cpp
    query/criteria/between_criteria.cpp
    query/criteria/binary_criteria.cpp
    query/criteria/check_null_criteria.cpp
    query/criteria/collection_criteria.cpp
    query/criteria/criteria_operators.cpp
    query/criteria/like_criteria.cpp
    query/criteria/logical_criteria.cpp
    query/criteria/not_criteria.cpp
    query/dialect/dialect.cpp
    query/dialect/dialect_builder.cpp
    query/expression/binary_column_expression.cpp
    query/expression/column_expression.cpp
    query/expression/expression_operators.cpp
    query/expression/placeholder_expression.cpp
    query/expression/table_column_expression.cpp
    query/expression/value_expression.cpp
    query/expression_evaluator.cpp
    query/interface/connection_impl.cpp
    query/interface/statement_impl.cpp
    query/interface/statement_proxy.cpp
    query/intermediates/alter_intermediate.cpp
    query/intermediates/alter_table_intermediate.cpp
    query/intermediates/create_intermediate.cpp
    query/intermediates/delete_from_intermediate.cpp
    query/intermediates/delete_intermediate.cpp
    query/intermediates/drop_intermediate.cpp
    query/intermediates/executable_query.cpp
    query/intermediates/execute_limit_intermediate.cpp
    query/intermediates/execute_offset_intermediate.cpp
    query/intermediates/execute_order_by_intermediate.cpp
    query/intermediates/execute_order_direction_intermediate.cpp
    query/intermediates/execute_where_intermediate.cpp
    query/intermediates/fetchable_query.cpp
    query/intermediates/from_intermediate.cpp
    query/intermediates/group_by_intermediate.cpp
    query/intermediates/insert_intermediate.cpp
    query/intermediates/intermediate.cpp
    query/intermediates/into_intermediate.cpp
    query/intermediates/join_intermediate.cpp
    query/intermediates/limit_intermediate.cpp
    query/intermediates/offset_intermediate.cpp
    query/intermediates/order_by_intermediate.cpp
    query/intermediates/order_direction_intermediate.cpp
    query/intermediates/select_intermediate.cpp
    query/intermediates/set_intermediate.cpp
    query/intermediates/update_intermediate.cpp
    query/intermediates/values_intermediate.cpp
    query/intermediates/where_intermediate.cpp
    query/internal/attribute_string_writer.cpp
    query/internal/basic_type_to_string_visitor.cpp
    query/internal/collection_utils.cpp
    query/internal/column_value_pair.cpp
    query/internal/criteria_transformer.cpp
    query/internal/identifier_discover_service.cpp
    query/internal/identifier_statement_binder.cpp
    query/internal/object_binder.cpp
    query/internal/object_binder.cpp
    query/internal/query_parts.cpp
    query/internal/query_result_impl.cpp
    query/internal/result_binder.cpp
    query/internal/schema_utils.cpp
    query/internal/select_query_builder.cpp
    query/internal/string_builder_utils.cpp
    query/internal/value_extractor.cpp
    query/mapping/abstract_pk_generator.cpp
    query/mapping/generators.cpp
    query/mapping/identity_pk_generator.cpp
    query/mapping/manual_pk_generator.cpp
    query/mapping/sequence_pk_generator.cpp
    query/mapping/table_pk_generator.cpp
    query/model/column.cpp
    query/model/column_options.cpp
    query/model/column_value.cpp
    query/model/constraint.cpp
    query/model/criteria_evaluator.cpp
    query/model/field.cpp
    query/model/foreign_key_options.cpp
    query/model/identifier.cpp
    query/model/primary_key_options.cpp
    query/model/record.cpp
    query/model/relation_endpoint.cpp
    query/model/table.cpp
    query/model/table.cpp
    query/observer.cpp
    query/resolver/abstract_joined_resolver.cpp
    query/resolver/joined_collection_resolver_producer.cpp
    query/resolver/producer_object_resolver_factory.cpp
    query/resolver/resolver_producer.cpp
    query/resolver_service.cpp
    query/runtime/backend_provider.cpp
    query/runtime/backend_provider.cpp
    query/runtime/connection.cpp
    query/runtime/connection_info.cpp
    query/runtime/connection_pool.cpp
    query/runtime/error_code.cpp
    query/runtime/query_result.cpp
    query/runtime/session.cpp
    query/runtime/statement.cpp
    query/runtime/statement_cache.cpp
    query/schema/basic_schema.cpp
    query/schema/basic_table_info.cpp
    query/schema/schema_node.cpp
    query/schema/schema_node_iterator.cpp
    query/schema/table_generator.cpp
    utils/default_type_traits.cpp
    utils/error.cpp
    utils/error_exception.cpp
    utils/errors.cpp
    utils/library.cpp
    utils/message_bus.cpp
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