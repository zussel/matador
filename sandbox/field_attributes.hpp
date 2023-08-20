#ifndef MATADOR_FIELD_ATTRIBUTES_HPP
#define MATADOR_FIELD_ATTRIBUTES_HPP

#include "contraints.hpp"

#include <iostream>

namespace matador {

struct field_attributes
{
  field_attributes() = default;
  field_attributes(size_t size) : size_(size) {}
  field_attributes(constraints options) : options_(options) {}
  field_attributes(size_t size, constraints options) : size_(size), options_(options) {}
  ~field_attributes() = default;

  friend std::ostream &operator<<(std::ostream &out, const field_attributes &attr)
  {
    if (attr.size_ > 0) {
      out << "size: " << attr.size_ << ", ";
    }
    out << "attributes:";
    for (const auto &c: constraints_to_name_map) {
      if (is_constraint_set(attr.options_, c.first)) {
        out << " " << c.second;
      }
    }
    return out;
  }

  size_t size_ = 0;
  constraints options_ = constraints::NONE;
};

}
#endif //MATADOR_FIELD_ATTRIBUTES_HPP
