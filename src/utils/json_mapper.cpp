#include "matador/utils/json_mapper.hpp"

namespace matador {

std::string json_mapper::to_string(const json &js, const json_format &format)
{
  return js.str(format);
}

json json_mapper::to_json(const std::string &str)
{
  return json_parser_.parse(str);
}

json json_mapper::to_json(const char *str)
{
  return json_parser_.parse(str);
}

}