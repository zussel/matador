#ifndef QUERY_ANY_TYPE_TO_STRING_VISITOR_HPP
#define QUERY_ANY_TYPE_TO_STRING_VISITOR_HPP

#include "matador/utils/types.hpp"
#include "matador/utils/placeholder.hpp"

#include "matador/sql/attribute_string_writer.hpp"

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
  explicit any_type_to_string_visitor(attribute_string_writer &writer, query_context &query);

  void operator()(const char &x) { result = writer->to_string(x); }
  void operator()(const short &x) { result = writer->to_string(x); }
  void operator()(const int &x) { result = writer->to_string(x); }
  void operator()(const long &x) { result = writer->to_string(x); }
  void operator()(const long long &x) { result = writer->to_string(x); }
  void operator()(const unsigned char &x) { result = writer->to_string(x); }
  void operator()(const unsigned short &x) { result = writer->to_string(x); }
  void operator()(const unsigned int &x) { result = writer->to_string(x); }
  void operator()(const unsigned long &x) { result = writer->to_string(x); }
  void operator()(const unsigned long long &x) { result = writer->to_string(x); }
  void operator()(const bool &x) { result = writer->to_string(x); }
  void operator()(const float &x) { result = writer->to_string(x); }
  void operator()(const double &x) { result = writer->to_string(x); }
  void operator()(const char *x) { result = writer->to_string(x); }
  void operator()(const std::string &x) { result = writer->to_string(x); }
  void operator()(const matador::date &x) { result = writer->to_string(x); }
  void operator()(const matador::time &x) { result = writer->to_string(x); }
  void operator()(const utils::blob &x) { result = writer->to_string(x); }
  void operator()(const utils::placeholder &x);

  attribute_string_writer *writer{};
  query_context &query;
  std::string result;
};

}
#endif //QUERY_ANY_TYPE_TO_STRING_VISITOR_HPP
