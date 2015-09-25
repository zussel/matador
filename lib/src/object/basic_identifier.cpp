//
// Created by sascha on 25.09.15.
//


#include "object/basic_identifier.hpp"

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

std::ostream &operator<<(std::ostream &os, const basic_identifier &x) {
  return x.print(os);
}
}