#include "matador/sql/dialect_builder.hpp"

namespace matador::sql {
dialect_builder &dialect_builder::builder()
{
  static dialect_builder instance;
  return instance;
}

dialect_builder &dialect_builder::create()
{
  dialect_ = {};
  return *this;
}

dialect_builder& dialect_builder::with_token_replace_map(const dialect::token_to_string_map &token_replace_map)
{
  for (const auto &token : token_replace_map) {
    dialect_.tokens_[token.first] = token.second;
  }

  return *this;
}

dialect_builder& dialect_builder::with_data_type_replace_map(const dialect::data_type_to_string_map &data_type_replace_map)
{
  for (const auto &data_type : data_type_replace_map) {
    dialect_.data_types_[data_type.first] = data_type.second;
  }

  return *this;
}

dialect_builder& dialect_builder::with_placeholder_func(const dialect::next_placeholder_func &func)
{
  dialect_.placeholder_func_ = func;

  return *this;
}

dialect_builder &dialect_builder::with_default_schema_name(const std::string &schema_name)
{
  dialect_.default_schema_name_ = schema_name;

  return *this;
}

dialect_builder &dialect_builder::with_compiler(std::unique_ptr<query_compiler> &&compiler)
{
  dialect_.compiler(std::move(compiler));

  return *this;
}

dialect dialect_builder::build()
{
  return std::move(dialect_);
}
}