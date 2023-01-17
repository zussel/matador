#include "matador/object/object_json_serializer.hpp"

#include "matador/utils/identifier.hpp"
#include "matador/utils/string.hpp"

namespace matador {

void object_json_serializer::on_primary_key(const char *id, identifier <std::string> &pk)
{
  if (pk.is_valid()) {
    result_[id] = pk.value();
  }

}

void object_json_serializer::on_attribute(const char *id, bool &x, long /*size*/)
{
  result_[id] = x;
}

void object_json_serializer::on_attribute(const char *id, std::string &x, long /*size*/)
{
  result_[id] = x;
}

void object_json_serializer::on_attribute(const char *id, date &to, long /*size*/)
{
  result_[id] = to_string(to);
}

void object_json_serializer::on_attribute(const char *id, time &to, long /*size*/)
{
  result_[id] = to_string(to);
}
}
