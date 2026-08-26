#ifndef QUERY_ANY_TYPE_TO_VISITOR_HPP
#define QUERY_ANY_TYPE_TO_VISITOR_HPP

#include "matador/utils/convert.hpp"

#include <cstdint>
#include <string>

namespace matador::utils {

template < typename Type >
class basic_type_converter {
public:
  static result<Type, conversion_error> convert_value(const database_type& from) {
    basic_type_converter converter;
    std::visit(converter, const_cast<database_type&>(from));

    return converter.result_;
  }

  void operator()(int8_t &x) { this->convert(x); }
  void operator()(int16_t &x) { this->convert(x); }
  void operator()(int32_t &x) { this->convert(x); }
  void operator()(int64_t &x) { this->convert(x); }
  void operator()(uint8_t &x) { this->convert(x); }
  void operator()(uint16_t &x) { this->convert(x); }
  void operator()(uint32_t &x) { this->convert(x); }
  void operator()(uint64_t &x) { this->convert(x); }
  void operator()(bool &x) { this->convert(x); }
  void operator()(float &x) { this->convert(x); }
  void operator()(double &x) { this->convert(x); }
  void operator()(const char *x) { this->convert(x); }
  void operator()(std::string &x) { this->convert(x); }
  void operator()(date_type_t &x) { this->convert(x); }
  void operator()(time_type_t &x) { this->convert(x); }
  void operator()(timestamp_type_t &x) { this->convert(x); }
  void operator()(blob_type_t &x) { this->convert(x); }

private:
  result<Type, conversion_error> result_{};

  template< typename FromType >
  void convert(FromType &from) {
    result_ = utils::to<Type>(from);
  }
};

}

#endif //QUERY_ANY_TYPE_TO_VISITOR_HPP
