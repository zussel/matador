#ifndef MATADOR_COLUMN_HPP
#define MATADOR_COLUMN_HPP

#include "matador/utils/field_attributes.hpp"
#include "matador/utils/any.hpp"

#include "matador/sql/types.hpp"

#include <memory>

namespace matador {

class column
{
public:
  column(std::string name, field_attributes attr)
  : name_(std::move(name))
  , attr_(attr) {}

  column(std::string name, matador::any val, field_attributes attr)
  : name_(std::move(name))
  , value_(std::move(val))
  , attr_(attr) {}

  template < typename Type >
  column(std::string name, const Type &val, field_attributes attr, typename std::enable_if<!std::is_same<Type, matador::any>::value>::type* = 0)
  : name_(std::move(name))
  , value_(val)
  , attr_(attr) {}

  template < class Operator >
  void process(Operator &op)
  {
    op.apply(name_, value_);
  }

  const std::string& name() const {
    return name_;
  }

  size_t size() const {
    return attr_.size();
  }

  template<typename Type>
  Type as() const {
    return value_.cast<Type>();
  }

private:
  std::string name_;
  matador::any value_;
  field_attributes attr_;
};

std::shared_ptr<column> make_column(const std::string &name, const field_attributes &attr = null_attributes, const matador::any& val = {}) {
  return std::make_shared<column>(name, val, attr);
}

template < typename Type >
std::shared_ptr<column> make_column(const std::string &name, const field_attributes &attr = null_attributes) {
  return make_column(name, Type{}, attr);
}

template < typename Type >
std::shared_ptr<column> make_column(const std::string &name, const Type &val, const field_attributes &attr = null_attributes) {
  return std::make_shared<column>(name, val, attr);
}

template < typename Type >
std::shared_ptr<column> make_pk_column(const std::string &name, size_t /*max_size*/ = 0) {
  return make_column(name, Type{}, constraints::PRIMARY_KEY);
}

template <>
std::shared_ptr<column> make_pk_column<std::string>(const std::string &name, size_t max_size) {
  return make_column<std::string>(name, { max_size, constraints::PRIMARY_KEY });
}


}
#endif //MATADOR_COLUMN_HPP
