#include "matador/object/json_object_mapper_serializer.hpp"

namespace matador {
namespace detail {

json_object_mapper_serializer::json_object_mapper_serializer(details::mapper_runtime &runtime_data)
  : runtime_data_(runtime_data)
{}

void json_object_mapper_serializer::serialize(const char *id, identifier<std::string> &pk)
{
  if (runtime_data_.key != id) {
    return;
  }
  if (!runtime_data_.value.is_string()) {
    return;
  }

  pk.value(runtime_data_.value.as<std::string>());
}

void json_object_mapper_serializer::serialize(const char *id, bool &to)
{
  if (runtime_data_.key != id) {
    return;
  }
  if (!runtime_data_.value.is_boolean()) {
    return;
  }
  to = runtime_data_.value.as<bool>();
}

void json_object_mapper_serializer::serialize(const char *id, std::string &to)
{
  if (runtime_data_.key != id) {
    return;
  }
  if (!runtime_data_.value.is_string()) {
    return;
  }
  to = runtime_data_.value.as<std::string>();
}

void json_object_mapper_serializer::serialize(const char *id, std::string &to, size_t)
{
  if (runtime_data_.key != id) {
    return;
  }
  if (!runtime_data_.value.is_string()) {
    return;
  }
  to = runtime_data_.value.as<std::string>();
}

void json_object_mapper_serializer::serialize(const char *id, date &to)
{
  if (runtime_data_.key != id) {
    return;
  }
  if (!runtime_data_.value.is_string()) {
    return;
  }
  to.set(runtime_data_.value.as<std::string>().c_str(), "%Y-%m-%d");
}

void json_object_mapper_serializer::serialize(const char *id, time &to)
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