#include "matador/object/object_json_serializer.hpp"

#include "matador/utils/string.hpp"

namespace matador {

void object_json_serializer::on_primary_key(const char *id, std::string &pk, size_t /*size*/)
{
  result_[id] = pk;
}

void object_json_serializer::on_attribute(const char *id, bool &x, const field_attributes &/*attr*/)
{
  result_[id] = x;
}

void object_json_serializer::on_attribute(const char *id, std::string &x, const field_attributes &/*attr*/)
{
  result_[id] = x;
}

void object_json_serializer::on_attribute(const char *id, date &to, const field_attributes &/*attr*/)
{
  result_[id] = to_string(to);
}

void object_json_serializer::on_attribute(const char *id, time &to, const field_attributes &/*attr*/)
{
  result_[id] = to_string(to);
}
}
