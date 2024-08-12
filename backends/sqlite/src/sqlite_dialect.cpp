#include "sqlite_dialect.hpp"

#include "matador/sql/dialect_builder.hpp"

[[maybe_unused]] const matador::sql::dialect *get_dialect()
{
  using namespace matador::sql;
  const static dialect d = dialect_builder::builder()
    .create()
    .with_token_replace_map({
                              {dialect::token_t::BEGIN,    "BEGIN TRANSACTION"},
                              {dialect::token_t::COMMIT,   "COMMIT TRANSACTION"},
                              {dialect::token_t::ROLLBACK, "ROLLBACK TRANSACTION"}
                            })
    .with_default_schema_name("main")
    .build();

  return &d;
}
