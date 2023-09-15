#include "matador/json/json_mapper_serializer.hpp"

namespace matador {
namespace detail {

void json_mapper_serializer::on_primary_key(const char *id, std::string &to, size_t /*size*/)
{
  if (runtime_data_.key != id) {
    return;
  }
  if (!runtime_data_.value.is_string()) {
    return;
  }

  to = runtime_data_.value.as<std::string>();
}

void json_mapper_serializer::on_revision(const char *id, unsigned long long int &rev)
{
  on_attribute(id, rev);
}

void json_mapper_serializer::on_attribute(const char *id, std::string &to, const field_attributes &/*attr*/)
{
  if (runtime_data_.key != id) {
    return;
  }
  if (!runtime_data_.value.is_string()) {
    return;
  }
  to = runtime_data_.value.as<std::string>();
}

void json_mapper_serializer::on_attribute(const char *id, bool &to, const field_attributes &/*attr*/)
{
  if (runtime_data_.key != id) {
    return;
  }
  if (!runtime_data_.value.is_boolean()) {
    return;
  }
  to = runtime_data_.value.as<bool>();
}

void json_mapper_serializer::on_attribute(const char *id, date &to, const field_attributes &/*attr*/)
{
  if (runtime_data_.key != id) {
    return;
  }
  if (!runtime_data_.value.is_string()) {
    return;
  }

  to = date::parse(runtime_data_.value.as<std::string>(), date_format::ISO8601);
}

void json_mapper_serializer::on_attribute(const char *id, time &to, const field_attributes &/*attr*/)
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