#ifndef QUERY_TYPES_HPP
#define QUERY_TYPES_HPP

#include "matador/query/mapping/access.hpp"

#include "matador/utils/types.hpp"

namespace matador::test {
struct types {
  unsigned int id_ = 0;
  int8_t char_ = 'c';
  short short_ = -127;
  int int_ = -65000;
  int64_t long64_ = -1234567890;
  unsigned char unsigned_char_ = 'H';
  unsigned short unsigned_short_ = 128;
  unsigned int unsigned_int_ = 65000;
  uint64_t unsigned_long64_ = 1234567890;
  float float_ = 3.1415f;
  double double_ = 1.1414;
  bool bool_ = true;
  std::string string_ = "Welt";
  std::string varchar_ = "Erde";
  //    matador::date date_;
  //    matador::time time_;
  utils::blob_type_t binary_{1, 2, 3, 4};

  template<class Operator>
  void process(Operator &op) {
    namespace field = query::access;
    using namespace matador::utils;
    field::primary_key(op, "id", id_);
    field::attribute(op, "val_char", char_);
    field::attribute(op, "val_float", float_);
    field::attribute(op, "val_double", double_);
    field::attribute(op, "val_short", short_);
    field::attribute(op, "val_int", int_);
    field::attribute(op, "val_long_long", long64_);
    field::attribute(op, "val_unsigned_char", unsigned_char_);
    field::attribute(op, "val_unsigned_short", unsigned_short_);
    field::attribute(op, "val_unsigned_int", unsigned_int_);
    field::attribute(op, "val_unsigned_long_long", unsigned_long64_);
    field::attribute(op, "val_bool", bool_);
    field::attribute(op, "val_string", string_);
    field::attribute(op, "val_varchar", varchar_, VarChar63);
    // field::attribute(op, "val_date", date_);
    // field::attribute(op, "val_time", time_);
    field::attribute(op, "val_binary", binary_);
  }
};
}

#endif // QUERY_TYPES_HPP
