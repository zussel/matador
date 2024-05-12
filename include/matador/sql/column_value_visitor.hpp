#ifndef MATADOR_COLUMN_VALUE_VISITOR_HPP
#define MATADOR_COLUMN_VALUE_VISITOR_HPP

#include "matador/utils/convert.hpp"
#include "matador/utils/types.hpp"

#include "matador/sql/placeholder.hpp"

#include <string>

namespace matador::sql {

template < typename Type >
struct column_value_visitor
{
  void operator()(char &x) { utils::convert(result, x); }
  void operator()(short &x) { utils::convert(result, x); }
  void operator()(int &x) { utils::convert(result, x); }
  void operator()(long &x) { utils::convert(result, x); }
  void operator()(long long &x) { utils::convert(result, x); }
  void operator()(unsigned char &x) { utils::convert(result, x); }
  void operator()(unsigned short &x) { utils::convert(result, x); }
  void operator()(unsigned int &x) { utils::convert(result, x); }
  void operator()(unsigned long &x) { utils::convert(result, x); }
  void operator()(unsigned long long &x) { utils::convert(result, x); }
  void operator()(bool &x) { utils::convert(result, x); }
  void operator()(float &x) { utils::convert(result, x); }
  void operator()(double &x) { utils::convert(result, x); }
  void operator()(const char *x) { utils::convert(result, x); }
  void operator()(std::string &x) { utils::convert(result, x); }
  void operator()(matador::date &x) { utils::convert(result, x); }
  void operator()(matador::time &x) { utils::convert(result, x); }
  void operator()(utils::blob &x) { utils::convert(result, x); }
  void operator()(placeholder &/*x*/) {}

  Type result{};
};

}

#endif //MATADOR_COLUMN_VALUE_VISITOR_HPP
