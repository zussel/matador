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
                            })
    .with_default_schema_name("")
    .build();
  return &d;
}
