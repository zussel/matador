#include "object_to_json_table_converter.hpp"

#include <matador/utils/string.hpp>

using namespace matador;

void object_to_json_table_converter::on_primary_key(const char *id, std::string &pk, size_t /*size*/)
{
  result_.push_back(create_value_information(pk, "PRIMARYKEY"));
}

void object_to_json_table_converter::on_attribute(const char *id, bool &to, const matador::field_attributes &)
{
  result_.push_back(create_value_information(to, "BOOLEAN"));
}

void object_to_json_table_converter::on_attribute(const char *id, std::string &to, const matador::field_attributes &)
{
  result_.push_back(create_value_information(to, "STRING"));
}

void object_to_json_table_converter::on_attribute(const char *id, matador::date &to, const matador::field_attributes &)
{
  result_.push_back(create_value_information(to_string(to), "DATE"));
}

void object_to_json_table_converter::on_attribute(const char *id, matador::time &to, const matador::field_attributes &)
{
  result_.push_back(create_value_information(to_string(to), "TIME"));
}
