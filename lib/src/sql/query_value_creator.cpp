#include "sql/query_value_creator.hpp"

namespace oos {
namespace detail {

template < class T >
oos::detail::basic_value* create_value(T &value)
{
  return new oos::value<T>(value);
}

oos::detail::basic_value* create_value(char *value, size_t len)
{
  return new oos::value<const char*>(value, len);
}

query_value_creator::query_value_creator()
{
  visitor_.register_visitor<char>(std::bind(&query_value_creator::process_char, this, std::placeholders::_1));
  visitor_.register_visitor<short>(std::bind(&query_value_creator::process_short, this, std::placeholders::_1));
  visitor_.register_visitor<int>(std::bind(&query_value_creator::process_int, this, std::placeholders::_1));
  visitor_.register_visitor<long>(std::bind(&query_value_creator::process_long, this, std::placeholders::_1));
  visitor_.register_visitor<unsigned char>(std::bind(&query_value_creator::process_uchar, this, std::placeholders::_1));
  visitor_.register_visitor<unsigned short>(std::bind(&query_value_creator::process_ushort, this, std::placeholders::_1));
  visitor_.register_visitor<unsigned int>(std::bind(&query_value_creator::process_uint, this, std::placeholders::_1));
  visitor_.register_visitor<unsigned long>(std::bind(&query_value_creator::process_ulong, this, std::placeholders::_1));
  visitor_.register_visitor<bool>(std::bind(&query_value_creator::process_bool, this, std::placeholders::_1));
  visitor_.register_visitor<float>(std::bind(&query_value_creator::process_float, this, std::placeholders::_1));
  visitor_.register_visitor<double>(std::bind(&query_value_creator::process_double, this, std::placeholders::_1));
  visitor_.register_visitor<oos::varchar_base>(std::bind(&query_value_creator::process_varchar, this, std::placeholders::_1));
  visitor_.register_visitor<std::string>(std::bind(&query_value_creator::process_string, this, std::placeholders::_1));
  visitor_.register_visitor<char*>(std::bind(&query_value_creator::process_charptr, this, std::placeholders::_1));
  visitor_.register_visitor<oos::time>(std::bind(&query_value_creator::process_time, this, std::placeholders::_1));
  visitor_.register_visitor<oos::date>(std::bind(&query_value_creator::process_date, this, std::placeholders::_1));
}

oos::detail::basic_value* query_value_creator::create_from_any(any &a)
{
  visitor_.visit(a);
  return value_;
}

void query_value_creator::process_char(char &val)
{
  value_ = create_value(val);
}

void query_value_creator::process_short(short &val)
{
  value_ = create_value(val);
}

void query_value_creator::process_int(int &val)
{
  value_ = create_value(val);
}

void query_value_creator::process_long(long &val)
{
  value_ = create_value(val);
}

void query_value_creator::process_uchar(unsigned char &val)
{
  value_ = create_value(val);
}

void query_value_creator::process_ushort(unsigned short &val)
{
  value_ = create_value(val);
}

void query_value_creator::process_uint(unsigned int &val)
{
  value_ = create_value(val);
}

void query_value_creator::process_ulong(unsigned long &val)
{
  value_ = create_value(val);
}

void query_value_creator::process_bool(bool &val)
{
  value_ = create_value(val);
}

void query_value_creator::process_float(float &val)
{
  value_ = create_value(val);
}

void query_value_creator::process_double(double &val)
{
  value_ = create_value(val);
}

void query_value_creator::process_charptr(char *val)
{
  value_ = create_value(val, strlen(val));
}

void query_value_creator::process_string(std::string &val)
{
  value_ = create_value(val);
}

void query_value_creator::process_varchar(oos::varchar_base &val)
{
  value_ = create_value(val);
}

void query_value_creator::process_time(oos::time &val)
{
  value_ = create_value(val);
}

void query_value_creator::process_date(oos::date &val)
{
  value_ = create_value(val);
}
}
}