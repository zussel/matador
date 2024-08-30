#include "odbc_dialect.hpp"

#include "matador/sql/dialect_builder.hpp"

[[maybe_unused]] const matador::sql::dialect *get_dialect() {
  using namespace matador::sql;
  const static dialect d = dialect_builder::builder()
    .create()
    .with_token_replace_map({
      {dialect_token::BEGIN_BINARY_DATA, "0x"},
      {dialect_token::END_BINARY_DATA, ""},
      {dialect_token::BEGIN, "BEGIN TRANSACTION"},
      {dialect_token::COMMIT, "COMMIT TRANSACTION"},
      {dialect_token::ROLLBACK, "ROLLBACK TRANSACTION"}
    })
    .with_data_type_replace_map({
      {matador::data_type::type_char, "SMALLINT"},
      {matador::data_type::type_int, "INT"},
      {matador::data_type::type_unsigned_char, "SMALLINT"},
      {matador::data_type::type_unsigned_short, "INT"},
      {matador::data_type::type_unsigned_long, "NUMERIC(21,0)"},
      {matador::data_type::type_unsigned_long_long, "NUMERIC(21,0)"},
      {matador::data_type::type_bool, "BIT"},
      {matador::data_type::type_float, "FLOAT(24)"},
      {matador::data_type::type_double, "FLOAT(53)"},
      {matador::data_type::type_text, "VARCHAR(MAX)"},
      {matador::data_type::type_time, "DATETIME2"},
      {matador::data_type::type_blob, "VARBINARY(MAX)"}
    })
    .with_default_schema_name("dbo")
    .build();

  return &d;
}
