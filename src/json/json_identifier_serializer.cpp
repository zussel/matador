#include "matador/json/json_identifier_serializer.hpp"

namespace matador {

namespace detail {

template < class T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
void to_json(T &val, std::string &out)
{
  out.append(std::to_string(val));
}

}

std::string json_identifier_serializer::serialize(const identifier &x)
{
  json_.clear();
  const_cast<identifier&>(x).serialize(*this);
  return json_;
}

void json_identifier_serializer::serialize(const char *, short &x)
{
  detail::to_json(x, json_);
}

void json_identifier_serializer::serialize(const char *, int &x)
{
  detail::to_json(x, json_);
}

void json_identifier_serializer::serialize(const char *, long &x)
{
  detail::to_json(x, json_);
}

void json_identifier_serializer::serialize(const char *, long long &x)
{
  detail::to_json(x, json_);
}

void json_identifier_serializer::serialize(const char *, unsigned short &x)
{
  detail::to_json(x, json_);
}

void json_identifier_serializer::serialize(const char *, unsigned int &x)
{
  detail::to_json(x, json_);
}

void json_identifier_serializer::serialize(const char *, unsigned long &x)
{
  detail::to_json(x, json_);
}

void json_identifier_serializer::serialize(const char *, unsigned long long &x)
{
  detail::to_json(x, json_);
}

void json_identifier_serializer::serialize(const char *, std::string &x)
{
  json_.append(x);
}

void json_identifier_serializer::serialize(const char *, null_type_t &)
{
  json_.append("null");
}

}
