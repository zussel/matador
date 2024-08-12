#ifndef QUERY_ANY_TYPE_TO_STRING_VISITOR_HPP
#define QUERY_ANY_TYPE_TO_STRING_VISITOR_HPP

#include "matador/utils/types.hpp"

#include "matador/utils/placeholder.hpp"

#include <string>

namespace matador {
class date;
class time;
}

namespace matador::sql {

class dialect;
struct query_context;

struct any_type_to_string_visitor
{
  explicit any_type_to_string_visitor(const dialect &d, query_context &query);

  void operator()(char &x) { to_string(x); }
  void operator()(short &x) { to_string(x); }
  void operator()(int &x) { to_string(x); }
  void operator()(long &x) { to_string(x); }
  void operator()(long long &x) { to_string(x); }
  void operator()(unsigned char &x) { to_string(x); }
  void operator()(unsigned short &x) { to_string(x); }
  void operator()(unsigned int &x) { to_string(x); }
  void operator()(unsigned long &x) { to_string(x); }
  void operator()(unsigned long long &x) { to_string(x); }
  void operator()(bool &x) { to_string(x); }
  void operator()(float &x) { to_string(x); }
  void operator()(double &x) { to_string(x); }
  void operator()(const char *x) { to_string(x); }
  void operator()(std::string &x) { to_string(x); }
  void operator()(matador::date &x) { to_string(x); }
  void operator()(matador::time &x) { to_string(x); }
  void operator()(utils::blob &x) { to_string(x); }
  void operator()(utils::placeholder &x) { to_string(x); }

  template<typename Type>
  void to_string(Type &val)
  {
    result = std::to_string(val);
  }
  void to_string(float &val);
  void to_string(double &val);
  void to_string(const char *val);
  void to_string(std::string &val);
  void to_string(matador::date &val);
  void to_string(matador::time &val);
  void to_string(utils::blob &val);
  void to_string(utils::placeholder &val);

  const dialect &d;
  query_context &query;
  std::string result;
};

}
#endif //QUERY_ANY_TYPE_TO_STRING_VISITOR_HPP
