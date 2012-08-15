#include "json/json_parser.hpp"

namespace oos {

json_parser::json_parser()
  : generic_json_parser<json_parser>(this)
{}

json_parser::~json_parser()
{}

void json_parser::on_begin_object()
{
}

void json_parser::on_object_key(const std::string &key)
{
}

void json_parser::on_end_object()
{
}

void json_parser::on_begin_array()
{
}

void json_parser::on_end_array()
{
}

void json_parser::on_string(const std::string &value)
{
}

void json_parser::on_number(double value)
{
}

void json_parser::on_bool(bool value)
{
}

void json_parser::on_null()
{
}

}