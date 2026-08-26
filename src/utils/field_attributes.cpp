#include "matador/utils/field_attributes.hpp"

namespace matador::utils {

field_attributes::field_attributes(const size_t size)
  : size_(size)
{}

field_attributes::field_attributes(const constraints options)
  : options_(options)
{}

field_attributes::field_attributes(const size_t size, const constraints options)
  : size_(size), options_(options)
{}

field_attributes& field_attributes::operator=(const size_t size) {
  size_ = size;
  options_ = constraints::None;
  return *this;
}
field_attributes& field_attributes::operator=(constraints opt) {
  options_ = opt;
  size_ = 0;
  return *this;
}

size_t field_attributes::size() const
{
  return size_;
}

constraints field_attributes::options() const
{
  return options_;
}

}