#include "matador/sql/key_value_pair.hpp"

#include <utility>

namespace matador::sql {

key_value_pair::key_value_pair(std::string name, utils::any_type value)
: name_(std::move(name))
, value_(std::move(value)) {
}

key_value_pair::key_value_pair(const column &col, utils::any_type value)
: name_(col.name)
, value_(std::move(value)) {
}

key_value_pair::key_value_pair(const char *name, utils::any_type value)
  : name_(name)
  , value_(std::move(value)) {
}

const std::string &key_value_pair::name() const {
  return name_;
}

const utils::any_type& key_value_pair::value() const {
  return value_;
}
}