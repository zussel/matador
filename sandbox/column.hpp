#ifndef MATADOR_COLUMN_HPP
#define MATADOR_COLUMN_HPP

#include "matador/utils/any.hpp"

#include "matador/sql/types.hpp"

#include "field_attributes.hpp"

#include <memory>

namespace matador {

class column
{
public:
  column(std::string name, matador::database_type type, field_attributes attr)
  : name_(std::move(name))
  , database_type_(type)
  , attr_(attr) {}

  column(std::string name, matador::database_type type, field_attributes attr, matador::any val)
  : name_(std::move(name))
  , value_(std::move(val))
  , database_type_(type)
  , attr_(attr) {}

  template < typename Type >
  column(std::string name, const Type &val, field_attributes attr)
  : name_(std::move(name))
  , value_(val)
  , database_type_(matador::data_type_traits<Type>::type())
  , attr_(attr) {}

  template<class Serializer>
  void serialize(Serializer &serializer)
  {
    serializer.apply(name_, value_);
  }

  const std::string& name() const {
    return name_;
  }

  matador::database_type database_type() const {
    return database_type_;
  }

  size_t size() const {
    return attr_.size_;
  }

  template<typename Type>
  Type as() const {
    return value_.cast<Type>();
  }

private:
  std::string name_;
  matador::any value_;
  matador::database_type database_type_;
  field_attributes attr_;
};

std::shared_ptr<column> make_column(const std::string &name, matador::database_type type, const field_attributes &attr = {}, const matador::any& val = {}) {
  return std::make_shared<column>(name, type, attr, val);
}

template < typename Type >
std::shared_ptr<column> make_column(const std::string &name, const field_attributes &attr = {}) {
  return make_column(name, matador::data_type_traits<Type>::type(), attr, matador::any(Type()));
}

template < typename Type >
std::shared_ptr<column> make_column(const std::string &name, const Type &val, const field_attributes &attr = {}) {
  return std::make_shared<column>(name, val, attr);
}

}
#endif //MATADOR_COLUMN_HPP
