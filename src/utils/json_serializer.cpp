#include "matador/utils/json_serializer.hpp"

namespace matador {

json_serializer::json_serializer(json_format format)
  : format_(format)
{}

void json_serializer::serialize(const char *id, identifier<std::string> &pk)
{
  write_id(id).append("\"").append(pk).append("\",");
  newline();
}

void json_serializer::serialize(const char *id, bool &val)
{
  write_id(id);
  if (val) {
    json_.append("true");
  } else {
    json_.append("false");
  }
  json_.append(",");
  newline();
}

void json_serializer::serialize(const char *id, std::string &val, size_t)
{
  write_id(id).append("\"").append(val).append("\",");
  newline();
}

void json_serializer::serialize(const char *id, std::string &val)
{
  write_id(id).append("\"").append(val).append("\",");
  newline();
}

void json_serializer::serialize(const char *id, date &d)
{
  write_id(id).append(matador::to_string(d)).append(",");
  newline();
}

void json_serializer::serialize(const char *id, time &t)
{
  write_id(id).append(matador::to_string(t)).append(",");
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
  if (depth_ > 0) {
    json_.append(",");
  }
  newline();
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

}