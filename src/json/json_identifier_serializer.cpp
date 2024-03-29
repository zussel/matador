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

void json_identifier_serializer::serialize(short &x, const field_attributes &/*attr*/)
{
  detail::to_json(x, json_);
}

void json_identifier_serializer::serialize(int &x, const field_attributes &/*attr*/)
{
  detail::to_json(x, json_);
}

void json_identifier_serializer::serialize(long &x, const field_attributes &/*attr*/)
{
  detail::to_json(x, json_);
}

void json_identifier_serializer::serialize(long long &x, const field_attributes &/*attr*/)
{
  detail::to_json(x, json_);
}

void json_identifier_serializer::serialize(unsigned short &x, const field_attributes &/*attr*/)
{
  detail::to_json(x, json_);
}

void json_identifier_serializer::serialize(unsigned int &x, const field_attributes &/*attr*/)
{
  detail::to_json(x, json_);
}

void json_identifier_serializer::serialize(unsigned long &x, const field_attributes &/*attr*/)
{
  detail::to_json(x, json_);
}

void json_identifier_serializer::serialize(unsigned long long &x, const field_attributes &/*attr*/)
{
  detail::to_json(x, json_);
}

void json_identifier_serializer::serialize(std::string &x, const field_attributes &/*attr*/)
{
  json_.append(x);
}

void json_identifier_serializer::serialize(null_type_t &, const field_attributes &/*attr*/)
{
  json_.append("null");
}

}
