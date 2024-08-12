#include "postgres_dialect.hpp"

#include "matador/sql/dialect_builder.hpp"

#include "matador/utils/data_types.hpp"

[[maybe_unused]] const matador::sql::dialect *get_dialect()
{
  using namespace matador::sql;
  const static dialect d = dialect_builder::builder()
    .create()
    .with_placeholder_func([](size_t index) {
      return "$" + std::to_string(index);
    })
    .with_token_replace_map({
                          {dialect::token_t::BEGIN_BINARY_DATA, "E'\\"}
                          })
    .with_data_type_replace_map({
                                  {matador::data_type::type_blob, "BYTEA"}
                                })
    .with_default_schema_name("public")
    .build();
  return &d;
}
