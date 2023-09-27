#ifndef OOS_VALUE_SERIALIZER_HPP
#define OOS_VALUE_SERIALIZER_HPP

#include "matador/utils/access.hpp"
#include "matador/utils/identifier.hpp"
#include "matador/utils/serializer.hpp"

#include "matador/sql/dialect_token.hpp"

namespace matador {

class sql;

namespace detail {

/// @cond MATADOR_DEV
class value_serializer;

class value_identifier_serializer : public identifier_serializer
{
public:
  explicit value_identifier_serializer(value_serializer &serializer)
  : serializer_(serializer) {}

  void serialize(short &value, const field_attributes &/*attr*/) override { extract_value(value); }
  void serialize(int &value, const field_attributes &/*attr*/) override { extract_value(value); }
  void serialize(long &value, const field_attributes &/*attr*/) override { extract_value(value); }
  void serialize(long long &value, const field_attributes &/*attr*/) override { extract_value(value); }
  void serialize(unsigned short &value, const field_attributes &/*attr*/) override { extract_value(value); }
  void serialize(unsigned int &value, const field_attributes &/*attr*/) override { extract_value(value); }
  void serialize(unsigned long &value, const field_attributes &/*attr*/) override { extract_value(value); }
  void serialize(unsigned long long &value, const field_attributes &/*attr*/) override { extract_value(value); }
  void serialize(std::string &value, const field_attributes &/*attr*/) override { extract_value(value); }
  void serialize(null_type_t &, const field_attributes &/*attr*/) override;

private:
  template < typename ValueType >
  void extract_value(ValueType &value);

private:
  value_serializer &serializer_;
};

//class OOS_SQL_API value_serializer
class value_serializer
{
public:
  value_serializer();
  ~value_serializer() = default;

  template<class T>
  values *execute(T &x) {
    values_ = std::make_unique<values>();
    matador::access::process(*this, x);
    return values_.release();
  }

  template < class T >
  void serialize(T &x)
  {
    matador::access::process(*this, x);
  }

  template<typename ValueType>
  void on_primary_key(const char *id, ValueType &x, typename std::enable_if<std::is_integral<ValueType>::value && !std::is_same<bool, ValueType>::value>::type* = 0)
  {
    on_attribute(id, x, { constraints::PRIMARY_KEY });
  }
  void on_primary_key(const char *id, std::string &pk, size_t size);
  void on_revision(const char *id, unsigned long long &rev);
  template < class Type >
  void on_attribute(const char *, Type &x, const field_attributes &/*attr*/ = null_attributes)
  {
    add_value(x);
  }
  void on_attribute(const char *id, char *x, const field_attributes &/*attr*/ = null_attributes);
  void on_attribute(const char *id, std::string &x, const field_attributes &/*attr*/ = null_attributes);
  void on_belongs_to(const char *id, identifiable_holder &x, cascade_type);
  void on_has_one(const char *id, identifiable_holder &x, cascade_type);
  void on_has_many(const char *, abstract_container &, const char *, const char *, cascade_type) {}
  void on_has_many(const char *, abstract_container &, cascade_type) {}

  template < class ValueType >
  void add_value(ValueType &val)
  {
    values_->push_back(std::make_shared<value>(val));
  }

  template < class ValueType >
  void add_value(ValueType &val, size_t size)
  {
    values_->push_back(std::make_shared<value>(val, size));
  }
private:
  std::unique_ptr<values> values_;
  value_identifier_serializer value_identifier_serializer_;
};

/// @endcond

template<typename ValueType>
void value_identifier_serializer::extract_value(ValueType &value)
{
  serializer_.add_value(value);
}

}
}
#endif //OOS_VALUE_SERIALIZER_HPP
