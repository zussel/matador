#include "postgres_dialect.hpp"

#include "matador/sql/dialect_builder.hpp"

#include "matador/utils/data_types.hpp"

[[maybe_unused]] const matador::sql::dialect *get_dialect()
{
  using namespace matador::sql;
  const static dialect d = dialect_builder::builder()
    .create()
    .with_placeholder_func([](const size_t index) {
      return "$" + std::to_string(index);
    })
    .with_token_replace_map({
                          {dialect_token::BEGIN_BINARY_DATA, "'"}
                          })
    .with_data_type_replace_map({
        {matador::data_type::type_char, "SMALLINT"},
        {matador::data_type::type_unsigned_char, "SMALLINT"},
        {matador::data_type::type_float, "REAL"},
        {matador::data_type::type_double, "DOUBLE PRECISION"},
        {matador::data_type::type_time, "TIMESTAMP"},
        {matador::data_type::type_blob, "BYTEA"}
    })
    .with_bool_strings("TRUE", "FALSE")
    .with_default_schema_name("public")
    .build();
  return &d;
}
