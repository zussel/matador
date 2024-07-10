#include "matador/object/object_json_attribute_extractor.hpp"

#include "matador/utils/string.hpp"

namespace matador {

void object_json_attribute_extractor::on_primary_key(const char *id, std::string &pk, size_t /*size*/)
{
  if (attributes_.count(id) > 0) {
    result_[id] = pk;
  }
}

void object_json_attribute_extractor::on_attribute(const char *id, bool &to, const field_attributes &/*attr*/)
{
  if (attributes_.count(id) > 0) {
    result_[id] = to;
  }
}

void object_json_attribute_extractor::on_attribute(const char *id, std::string &to, const field_attributes &/*attr*/)
{
  if (attributes_.count(id) > 0) {
    result_[id] = to;
  }
}

void object_json_attribute_extractor::on_attribute(const char *id, date &to, const field_attributes &/*attr*/)
{
  if (attributes_.count(id) > 0) {
    result_[id] = to_string(to);
  }
}

void object_json_attribute_extractor::on_attribute(const char *id, time &to, const field_attributes &/*attr*/)
{
  if (attributes_.count(id) > 0) {
    result_[id] = to_string(to);
  }
}

}