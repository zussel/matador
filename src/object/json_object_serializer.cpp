#include "matador/object/json_object_serializer.hpp"

namespace matador {

void json_object_serializer::serialize(const char *id, identifier<std::string> &pk)
{
  write_id(id);
  append(pk.value());
  json_.append(",");
  newline();
}

void json_object_serializer::serialize(const char *id, bool &val)
{
  write_id(id);
  append(val);
  json_.append(",");
  newline();
}

void json_object_serializer::serialize(const char *id, std::string &val)
{
  if (val.empty()) {
    return;
  }
  write_id(id);
  append(val);
  json_.append(",");
  newline();
}

void json_object_serializer::serialize(const char *id, std::string &val, size_t)
{
  if (val.empty()) {
    return;
  }
  write_id(id);
  append(val);
  json_.append(",");
  newline();
}

void json_object_serializer::serialize(const char *id, const char *val, size_t len)
{
  if (val == nullptr) {
    return;
  }
  write_id(id);
  append(val);
  json_.append(",");
  newline();
}

void json_object_serializer::serialize(const char *id, date &d)
{
  if (d.julian_date() == 0) {
    return;
  }
  write_id(id);
  append(matador::to_string(d));
  json_.append(",");
  newline();
}

void json_object_serializer::serialize(const char *id, time &t)
{
  if (t.get_timeval().tv_sec == 0 && t.get_timeval().tv_usec == 0) {
    return;
  }
  write_id(id);
  append(matador::to_string(t));
  json_.append(",");
  newline();
}

void json_object_serializer::write_id(const char *id)
{
  indent();
  json_.append("\"").append(id).append("\": ");
}

void json_object_serializer::begin_object()
{
  json_.append("{");
  newline();
  ++depth_;
}

void json_object_serializer::end_object()
{
  --depth_;
  indent();
  json_.append("}");
  if (depth_ > 0) {
    json_.append(",");
  }
}

void json_object_serializer::indent()
{
  json_.append(depth_ * format_.indentation(), ' ');
}

void json_object_serializer::newline()
{
  if (format_.show_line_break()) {
    json_.append("\n");
  }
}

void json_object_serializer::append(const char *str)
{
  json_.append("\"").append(str).append("\"");
}

void json_object_serializer::append(const std::string &str)
{
  json_.append("\"").append(str).append("\"");
}

void json_object_serializer::append(std::string &val, size_t)
{
  append(val);
}

void json_object_serializer::append(const char *val, size_t)
{
  append(val);
}

constexpr char const* bool_to_string(bool b)
{
  return b ? "true" : "false";
}

void json_object_serializer::append(const bool &value)
{
  json_.append(bool_to_string(value));
}

}
