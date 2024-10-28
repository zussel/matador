#ifndef QUERY_DIALECT_BUILDER_HPP
#define QUERY_DIALECT_BUILDER_HPP

#include "matador/sql/dialect.hpp"

namespace matador::sql {

class dialect_builder
{
public:
  static dialect_builder& builder();

  dialect_builder& create();

  dialect_builder& with_token_replace_map(const dialect::token_to_string_map &token_replace_map);
  dialect_builder& with_data_type_replace_map(const dialect::data_type_to_string_map &data_type_replace_map);
  dialect_builder& with_placeholder_func(const dialect::next_placeholder_func &func);
  dialect_builder& with_default_schema_name(const std::string &schema_name);
  dialect_builder& with_bool_strings(const std::string &true_string, const std::string &false_string);
  dialect_builder& with_compiler(std::unique_ptr<query_compiler> &&compiler);

  dialect build();

private:
  dialect_builder() = default;

private:
  dialect dialect_;
};

}
#endif //QUERY_DIALECT_BUILDER_HPP
