#include "matador/json/json_serializer.hpp"

namespace matador {

void json_serializer::on_primary_key(const char *id, std::string &pk, size_t /*size*/)
{
  write_id(id);
  append(pk).append(",");
  newline();
}

void json_serializer::on_revision(const char *id, unsigned long long int &rev)
{
  on_attribute(id, rev);
}

void json_serializer::on_attribute(const char *id, bool &val, const field_attributes &/*attr*/)
{
  write_id(id);
  append(val).append(",");
  newline();
}

void json_serializer::on_attribute(const char *id, std::string &val, const field_attributes &/*attr*/)
{
  if (val.empty()) {
    return;
  }
  write_id(id);
  append(val).append(",");
  newline();
}

void json_serializer::on_attribute(const char *id, date &d, const field_attributes &/*attr*/)
{
  if (d.julian_date() == 0) {
    return;
  }
  write_id(id);
  append(matador::to_string(d)).append(",");
  newline();
}

void json_serializer::on_attribute(const char *id, time &t, const field_attributes &/*attr*/)
{
  if (t.get_time_info().seconds_since_epoch == 0 || t.get_time_info().milliseconds == 0) {
    return;
  }
  write_id(id);
  append(matador::to_string(t)).append(",");
  newline();
}

std::string &json_serializer::write_id(const char *id)
{
  indent();
  json_.append("\"").append(id).append("\": ");
  return json_;
}

void json_serializer::begin_object()
{
  json_.append("{");
  newline();
  ++depth_;
}

void json_serializer::end_object()
{
  --depth_;
  indent();
  json_.append("}");
  if (depth_ > 0 && !is_array) {
    json_.append(",");
  }
}

void json_serializer::begin_array()
{
  json_.append("[");
  is_array = true;
}

void json_serializer::end_array()
{
  json_.append("]");
  if (depth_ > 0) {
    json_.append(",");
  }
  is_array = false;
}

void json_serializer::indent()
{
  json_.append(depth_ * format_.indentation(), ' ');
}

void json_serializer::newline()
{
  if (format_.show_line_break()) {
    json_.append("\n");
  }
}

std::string &json_serializer::append(const char *str)
{
  json_.append("\"").append(str).append("\"");
  return json_;
}

std::string &json_serializer::append(const std::string &str)
{
  json_.append("\"").append(str).append("\"");
  return json_;
}

std::string &json_serializer::append(std::string &val, size_t)
{
  return append(val);
}

constexpr char const* bool_to_string(bool b)
{
  return b ? "true" : "false";
}

std::string &json_serializer::append(const bool &value)
{
  return json_.append(bool_to_string(value));
}

}