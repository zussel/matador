#ifndef OOS_VALUE_COLUMN_SERIALIZER_HPP
#define OOS_VALUE_COLUMN_SERIALIZER_HPP

#include "matador/utils/access.hpp"
#include "matador/utils/identifier.hpp"
#include "matador/utils/serializer.hpp"

#include "matador/sql/column.hpp"

namespace matador {

class sql;

namespace detail {

/// @cond MATADOR_DEV
class value_column_serializer;

class value_column_identifier_serializer : public identifier_serializer
{
public:
  explicit value_column_identifier_serializer(value_column_serializer &serializer)
  : serializer_(serializer) {}

  void serialize(short &value, const field_attributes &attr) override { extract_value(value, attr); }
  void serialize(int &value, const field_attributes &attr) override { extract_value(value, attr); }
  void serialize(long &value, const field_attributes &attr) override { extract_value(value, attr); }
  void serialize(long long &value, const field_attributes &attr) override { extract_value(value, attr); }
  void serialize(unsigned short &value, const field_attributes &attr) override { extract_value(value, attr); }
  void serialize(unsigned int &value, const field_attributes &attr) override { extract_value(value, attr); }
  void serialize(unsigned long &value, const field_attributes &attr) override { extract_value(value, attr); }
  void serialize(unsigned long long &value, const field_attributes &attr) override { extract_value(value, attr); }
  void serialize(std::string &value, const field_attributes &attr) override { extract_value(value, attr); }
  void serialize(null_type_t &, const field_attributes &/*attr*/) override {}

private:
  template < typename ValueType >
  void extract_value(ValueType &value, const field_attributes &attr);

private:
  value_column_serializer &serializer_;
};

class value_column_serializer
{
public:
  value_column_serializer();

  template<class T>
  void append_to(const std::shared_ptr<columns> &cols, T &x)
  {
    cols_ = cols;
    matador::access::serialize(*this, x);
  }

  template < class T >
  void serialize(T &x)
  {
    matador::access::serialize(*this, x);
  }

  void on_attribute(const char *id, char &x, const field_attributes &attr = null_attributes);
  void on_attribute(const char *id, short &x, const field_attributes &attr = null_attributes);
  void on_attribute(const char *id, int &x, const field_attributes &attr = null_attributes);
  void on_attribute(const char *id, long &x, const field_attributes &attr = null_attributes);
  void on_attribute(const char *id, long long &x, const field_attributes &attr = null_attributes);
  void on_attribute(const char *id, unsigned char &x, const field_attributes &attr = null_attributes);
  void on_attribute(const char *id, unsigned short &x, const field_attributes &attr = null_attributes);
  void on_attribute(const char *id, unsigned int &x, const field_attributes &attr = null_attributes);
  void on_attribute(const char *id, unsigned long &x, const field_attributes &attr = null_attributes);
  void on_attribute(const char *id, unsigned long long &x, const field_attributes &attr = null_attributes);
  void on_attribute(const char *id, float &x, const field_attributes &attr = null_attributes);
  void on_attribute(const char *id, double &x, const field_attributes &attr = null_attributes);
  void on_attribute(const char *id, bool &x, const field_attributes &attr = null_attributes);
  void on_attribute(const char *id, char *x, const field_attributes &attr = null_attributes);
  void on_attribute(const char *id, std::string &x, const field_attributes &attr = null_attributes);
  void on_attribute(const char *id, date &x, const field_attributes &attr = null_attributes);
  void on_attribute(const char *id, time &x, const field_attributes &attr = null_attributes);
  void on_belongs_to(const char *id, identifiable_holder &x, cascade_type);
  void on_has_one(const char *id, identifiable_holder &x, cascade_type);
  template< typename ValueType >
  void on_primary_key(const char *id, ValueType &x, const field_attributes &attr = null_attributes)
  {
    on_attribute(id, x, attr);
  }
  void on_has_many(const char *, abstract_has_many &, const char *, const char *, cascade_type) {}
  void on_has_many(const char *, abstract_has_many &, cascade_type) {}

  template < class ValueType >
  void add_column_value(const char *col, ValueType &val, const field_attributes &attr)
  {
    cols_->push_back(make_value_column(col, val, attr));
  }

  template < class ValueType >
  void add_value(const char *col, ValueType &val, size_t size)
  {
    cols_->push_back(std::make_shared<value>(col, val, size));
  }

private:
  std::shared_ptr<columns> cols_;
  value_column_identifier_serializer value_column_identifier_serializer_;
};

/// @endcond

template<typename ValueType>
void value_column_identifier_serializer::extract_value(ValueType &value, const field_attributes &attr)
{
  // Todo: Add column name
  serializer_.add_column_value("", value, attr);
}

}
}
#endif //OOS_VALUE_COLUMN_SERIALIZER_HPP
