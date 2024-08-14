#include "sqlite_dialect.hpp"
#include "sqlite_query_compiler.hpp"

#include "matador/sql/dialect_builder.hpp"

[[maybe_unused]] const matador::sql::dialect *get_dialect()
{
  using namespace matador::sql;
  using namespace matador::backends::sqlite;
  const static dialect d = dialect_builder::builder()
    .create()
    .with_token_replace_map({
                              {dialect_token::BEGIN,    "BEGIN TRANSACTION"},
                              {dialect_token::COMMIT,   "COMMIT TRANSACTION"},
                              {dialect_token::ROLLBACK, "ROLLBACK TRANSACTION"}
                            })
    .with_default_schema_name("main")
    .with_compiler(std::make_unique<sqlite_query_compiler>())
    .build();

  return &d;
}
