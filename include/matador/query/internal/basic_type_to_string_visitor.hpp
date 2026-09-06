#ifndef QUERY_ANY_TYPE_TO_STRING_VISITOR_HPP
#define QUERY_ANY_TYPE_TO_STRING_VISITOR_HPP

#include "matador/utils/types.hpp"

#include "matador/query/internal/attribute_string_writer.hpp"

#include <string>

namespace matador::query {
class dialect;
struct query_context;
}

namespace matador::query::internal {
struct basic_type_to_string_visitor {
  explicit basic_type_to_string_visitor(attribute_string_writer &writer, query_context &query);

  void operator()(const int8_t &x) { result = writer->to_string(x); }
  void operator()(const int16_t &x) { result = writer->to_string(x); }
  void operator()(const int32_t &x) { result = writer->to_string(x); }
  void operator()(const int64_t &x) { result = writer->to_string(x); }
  void operator()(const uint8_t &x) { result = writer->to_string(x); }
  void operator()(const uint16_t &x) { result = writer->to_string(x); }
  void operator()(const uint32_t &x) { result = writer->to_string(x); }
  void operator()(const uint64_t &x) { result = writer->to_string(x); }
  void operator()(const bool &x) { result = writer->to_string(x); }
  void operator()(const float &x) { result = writer->to_string(x); }
  void operator()(const double &x) { result = writer->to_string(x); }
  void operator()(const char *x) { result = writer->to_string(x); }
  void operator()(const std::string &x) { result = writer->to_string(x); }
  void operator()(const utils::date_type_t &x) { result = writer->to_string(x); }
  void operator()(const utils::time_type_t &x) { result = writer->to_string(x); }
  void operator()(const utils::timestamp_type_t &x) { result = writer->to_string(x); }
  void operator()(const utils::blob_type_t &x) { result = writer->to_string(x); }

  attribute_string_writer *writer{};
  query_context &query;
  std::string result;
};

}
#endif //QUERY_ANY_TYPE_TO_STRING_VISITOR_HPP
