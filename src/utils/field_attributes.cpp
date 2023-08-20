#include "matador/utils/field_attributes.hpp"

namespace matador {

field_attributes::field_attributes(size_t size)
  : size_(size)
{}

field_attributes::field_attributes(constraints options)
  : options_(options)
{}

field_attributes::field_attributes(size_t size, constraints options)
  : size_(size)
  , options_(options)
{}

size_t field_attributes::size() const
{
  return size_;
}

constraints field_attributes::options() const
{
  return options_;
}

}