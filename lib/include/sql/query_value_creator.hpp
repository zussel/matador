#ifndef OOS_QUERY_VALUE_CREATOR_HPP
#define OOS_QUERY_VALUE_CREATOR_HPP

#include "sql/value.hpp"

#include "tools/any.hpp"
#include "tools/any_visitor.hpp"

#include <memory>

namespace oos {
namespace detail {

class query_value_creator
{
public:
  query_value_creator();

  oos::detail::basic_value* create_from_any(any &a);

private:
  void process_char(char &val);
  void process_short(short &val);
  void process_int(int &val);
  void process_long(long &val);
  void process_uchar(unsigned char &val);
  void process_ushort(unsigned short &val);
  void process_uint(unsigned int &val);
  void process_ulong(unsigned long &val);
  void process_bool(bool &val);
  void process_float(float &val);
  void process_double(double &val);
  void process_charptr(char *val);
  void process_string(std::string &val);
  void process_varchar(oos::varchar_base &val);
  void process_time(oos::time &val);
  void process_date(oos::date &val);

private:
  any_visitor visitor_;
  oos::detail::basic_value* value_ = nullptr;
};
}
}
#endif //OOS_QUERY_VALUE_CREATOR_HPP
