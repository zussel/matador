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

  void operator()(const char &x) { to_string(x); }
  void operator()(const short &x) { to_string(x); }
  void operator()(const int &x) { to_string(x); }
  void operator()(const long &x) { to_string(x); }
  void operator()(const long long &x) { to_string(x); }
  void operator()(const unsigned char &x) { to_string(x); }
  void operator()(const unsigned short &x) { to_string(x); }
  void operator()(const unsigned int &x) { to_string(x); }
  void operator()(const unsigned long &x) { to_string(x); }
  void operator()(const unsigned long long &x) { to_string(x); }
  void operator()(const bool &x) { to_string(x); }
  void operator()(const float &x) { to_string(x); }
  void operator()(const double &x) { to_string(x); }
  void operator()(const char *x) { to_string(x); }
  void operator()(const std::string &x) { to_string(x); }
  void operator()(const matador::date &x) { to_string(x); }
  void operator()(const matador::time &x) { to_string(x); }
  void operator()(const utils::blob &x) { to_string(x); }
  void operator()(const utils::placeholder &x) { to_string(x); }

  template<typename Type>
  void to_string(const Type &val)
  {
    result = std::to_string(val);
  }
  void to_string(const bool &val);
  void to_string(const float &val);
  void to_string(const double &val);
  void to_string(const char *val);
  void to_string(const std::string &val);
  void to_string(const matador::date &val);
  void to_string(const matador::time &val);
  void to_string(const utils::blob &val);
  void to_string(const utils::placeholder &val);

  const dialect &d;
  query_context &query;
  std::string result;
};

}
#endif //QUERY_ANY_TYPE_TO_STRING_VISITOR_HPP
