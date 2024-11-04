#ifndef QUERY_ANY_TYPE_TO_VISITOR_HPP
#define QUERY_ANY_TYPE_TO_VISITOR_HPP

#include "matador/utils/convert.hpp"

#include <string>

namespace matador {
class date;
class time;
}

namespace matador::utils {

template < typename Type >
struct any_type_converter
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
  void operator()(date &x) { utils::convert(result, x); }
  void operator()(time &x) { utils::convert(result, x); }
  void operator()(utils::blob &x) { utils::convert(result, x); }
  void operator()(utils::placeholder &/*x*/) {}

  Type result{};
};

}

#endif //QUERY_ANY_TYPE_TO_VISITOR_HPP
