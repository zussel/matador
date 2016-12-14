//
// Created by sascha on 25.09.15.
//


#include "oos/utils/basic_identifier.hpp"

#include <iosfwd>

namespace oos {
basic_identifier::basic_identifier() {

}

basic_identifier::~basic_identifier() {

}

bool basic_identifier::operator==(const basic_identifier &x) const {

  return equal_to(x);
}

bool basic_identifier::operator!=(const basic_identifier &x) const {
  return !this->operator==(x);
}

bool basic_identifier::operator<(const basic_identifier &x) const {
  return less(x);
}

bool basic_identifier::as_value() const
{
  return as_value_;
}

void basic_identifier::as_value(bool asvalue)
{
  as_value_ = asvalue;
}

std::ostream &operator<<(std::ostream &os, const basic_identifier &x) {
  return x.print(os);
}
}