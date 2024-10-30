#include "mysql_dialect.hpp"

#include "matador/sql/dialect_builder.hpp"

[[maybe_unused]] const matador::sql::dialect *get_dialect()
{
  using namespace matador::sql;
  const static dialect d = dialect_builder::builder()
    .create()
    .with_token_replace_map({
                              {dialect_token::START_QUOTE, "`"},
                              {dialect_token::END_QUOTE, "`"},
                              {dialect_token::BEGIN_BINARY_DATA, "'"}
                            })
    .with_data_type_replace_map({
                                {matador::data_type::type_unsigned_char, "TINYINT UNSIGNED"},
                                {matador::data_type::type_unsigned_long, "BIGINT UNSIGNED"},
                                {matador::data_type::type_unsigned_long_long, "BIGINT UNSIGNED"},
                                {matador::data_type::type_bool, "BIT(3)"},
                                {matador::data_type::type_time, "DATETIME(3)"}
    })
    .with_default_schema_name("")
    .build();
  return &d;
}
