#include "matador/utils/json_mapper_serializer.hpp"

namespace matador {
namespace detail {

void json_mapper_serializer::serialize(const char *id, identifier<std::string> &pk)
{
  if (runtime_data_.key != id) {
    return;
  }
  if (!runtime_data_.value.is_string()) {
    return;
  }

  pk.value(runtime_data_.value.as<std::string>());
}

void json_mapper_serializer::serialize(const char *id, std::string &to)
{
  if (runtime_data_.key != id) {
    return;
  }
  if (!runtime_data_.value.is_string()) {
    return;
  }
  to = runtime_data_.value.as<std::string>();
}

void json_mapper_serializer::serialize(const char *id, bool &to)
{
  if (runtime_data_.key != id) {
    return;
  }
  if (!runtime_data_.value.is_boolean()) {
    return;
  }
  to = runtime_data_.value.as<bool>();
}

void json_mapper_serializer::serialize(const char *id, std::string &to, size_t)
{
  if (runtime_data_.key != id) {
    return;
  }
  if (!runtime_data_.value.is_string()) {
    return;
  }
  to = runtime_data_.value.as<std::string>();
}

void json_mapper_serializer::serialize(const char *id, date &to)
{
  if (runtime_data_.key != id) {
    return;
  }
  if (!runtime_data_.value.is_string()) {
    return;
  }

  to = date::parse(runtime_data_.value.as<std::string>(), date_format::ISO8601);
}

void json_mapper_serializer::serialize(const char *id, time &to)
{
  if (runtime_data_.key != id) {
    return;
  }
  if (!runtime_data_.value.is_string()) {
    return;
  }

  to = time::parse(runtime_data_.value.as<std::string>(), "%Y-%m-%d %H:%M:%S");
}

}
}