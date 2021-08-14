#include "matador/object/object_json_serializer.hpp"

#include "matador/utils/identifier.hpp"
#include "matador/utils/string.hpp"

namespace matador {

void object_json_serializer::serialize(const char *id, identifier <std::string> &pk)
{
  if (pk.is_valid()) {
    result_[id] = pk.value();
  }

}

void object_json_serializer::serialize(const char *id, bool &x)
{
  result_[id] = x;
}

void object_json_serializer::serialize(const char *id, std::string &x)
{
  result_[id] = x;
}

void object_json_serializer::serialize(const char *id, std::string &x, size_t)
{
  result_[id] = x;
}

void object_json_serializer::serialize(const char *id, date &to)
{
  result_[id] = to_string(to);
}

void object_json_serializer::serialize(const char *id, time &to)
{
  result_[id] = to_string(to);
}
}
