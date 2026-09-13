#include "matador/query/dialect_builder.hpp"

namespace matador::query {
dialect_builder &dialect_builder::builder() {
  static dialect_builder instance;
  return instance;
}

dialect_builder &dialect_builder::create() {
  dialect_ = {};
  return *this;
}

dialect_builder &dialect_builder::with_token_replace_map(const dialect::token_to_string_map &token_replace_map) {
  for (const auto &[token, name]: token_replace_map) {
    dialect_.tokens_[token] = name;
  }

  return *this;
}

dialect_builder &dialect_builder::with_data_type_replace_map(
  const dialect::data_type_to_string_map &data_type_replace_map) {
  for (const auto &[type, name]: data_type_replace_map) {
    dialect_.data_types_[type] = name;
  }

  return *this;
}

dialect_builder &dialect_builder::with_placeholder_func(const dialect::next_placeholder_func &func) {
  dialect_.placeholder_func_ = func;

  return *this;
}

dialect_builder &dialect_builder::with_default_schema_name(const std::string &schema_name) {
  dialect_.default_schema_name_ = schema_name;

  return *this;
}

dialect_builder &dialect_builder::with_bool_strings(const std::string &true_string, const std::string &false_string) {
  dialect_.bool_strings(true_string, false_string);

  return *this;
}

dialect_builder &dialect_builder::with_escape_string_func(const dialect::to_escaped_string_func &func) {
  dialect_.to_escaped_string_func_ = func;

  return *this;
}

dialect dialect_builder::build() {
  return std::move(dialect_);
}
}
