#ifndef MATADOR_FIELD_ATTRIBUTES_HPP
#define MATADOR_FIELD_ATTRIBUTES_HPP

#include "matador/utils/contraints.hpp"

namespace matador {

class field_attributes
{
public:
  field_attributes() = default;
  field_attributes(size_t size); // NOLINT(*-explicit-constructor)
  field_attributes(constraints options); // NOLINT(*-explicit-constructor)
  field_attributes(size_t size, constraints options);
  field_attributes(const field_attributes &x) = default;
  field_attributes& operator=(const field_attributes &x) = default;
  field_attributes(field_attributes &&x) = default;
  field_attributes& operator=(field_attributes &&x) = default;
  ~field_attributes() = default;

  size_t size() const;
  constraints options() const;

private:
  size_t size_ = 0;
  constraints options_ = constraints::NONE;
};

const field_attributes null_attributes {};

}
#endif //MATADOR_FIELD_ATTRIBUTES_HPP
