#ifndef QUERY_VALUE_EXTRACTOR_HPP
#define QUERY_VALUE_EXTRACTOR_HPP

#include "matador/sql/fk_value_extractor.hpp"

#include "matador/object/attribute_writer.hpp"
#include "matador/object/data_type_traits.hpp"

#include "matador/utils/foreign_attributes.hpp"

#include <vector>

namespace matador::sql {

class value_extractor final : public object::attribute_writer
{
private:
  explicit value_extractor(std::vector<utils::any_type> &values);

public:
  template < class Type >
  static std::vector<utils::any_type> extract(const Type &type)
  {
    std::vector<utils::any_type> values;
    value_extractor gen(values);
    access::process(gen, type);
    return values;
  }

  template<typename ValueType>
  void on_primary_key(const char *, ValueType &x, std::enable_if_t<std::is_integral_v<ValueType> && !std::is_same_v<bool, ValueType>>* = nullptr)
  {
    object::data_type_traits<ValueType>::bind_value(*this, 0, x);
  }
  void on_primary_key(const char *id, std::string &pk, size_t size);
  void on_revision(const char *id, unsigned long long &rev);
  template < class Type >
  void on_attribute(const char *, Type &x, const utils::field_attributes &/*attr*/ = utils::null_attributes)
  {
    object::data_type_traits<Type>::bind_value(*this, 0, x);
  }
  void on_attribute(const char *id, char *x, const utils::field_attributes &/*attr*/ = utils::null_attributes);
  void on_attribute(const char *id, std::string &x, const utils::field_attributes &/*attr*/ = utils::null_attributes);

  template<class Type, template < class ... > class Pointer>
  void on_belongs_to(const char * /*id*/, Pointer<Type> &x, const utils::foreign_attributes &/*attr*/)
  {
    values_.emplace_back(fk_value_extractor_.extract(*x));
  }
  template<class Type, template < class ... > class Pointer>
  void on_has_one(const char * /*id*/, Pointer<Type> &x, const utils::foreign_attributes &/*attr*/)
  {
    values_.emplace_back(fk_value_extractor_.extract(*x));
  }
  template<class ContainerType>
  void on_has_many(const char * /*id*/, ContainerType &, const char *, const utils::foreign_attributes &/*attr*/) {}
  template<class ContainerType>
  void on_has_many_to_many(const char * /*id*/,
                           ContainerType &/*c*/,
                           const char * /*join_column*/,
                           const char * /*inverse_join_column*/,
                           const utils::foreign_attributes &/*attr*/) {}
  template<class ContainerType>
  void on_has_many_to_many(const char * /*id*/,
                           ContainerType &/*c*/,
                           const utils::foreign_attributes &/*attr*/) {}

public:
  void write_value(size_t pos, const char &x) override;
  void write_value(size_t pos, const short &x) override;
  void write_value(size_t pos, const int &x) override;
  void write_value(size_t pos, const long &x) override;
  void write_value(size_t pos, const long long int &x) override;
  void write_value(size_t pos, const unsigned char &x) override;
  void write_value(size_t pos, const unsigned short &x) override;
  void write_value(size_t pos, const unsigned int &x) override;
  void write_value(size_t pos, const unsigned long &x) override;
  void write_value(size_t pos, const unsigned long long int &x) override;
  void write_value(size_t pos, const bool &x) override;
  void write_value(size_t pos, const float &x) override;
  void write_value(size_t pos, const double &x) override;
  void write_value(size_t pos, const time &x ) override;
  void write_value(size_t pos, const date &x ) override;
  void write_value(size_t pos, const char *x) override;
  void write_value(size_t pos, const char *x, size_t size) override;
  void write_value(size_t pos, const std::string &x) override;
  void write_value(size_t pos, const std::string &x, size_t size) override;
  void write_value(size_t pos, const utils::blob &x) override;

private:
  detail::fk_value_extractor fk_value_extractor_;
  std::vector<utils::any_type> &values_;
};

}

#endif //QUERY_VALUE_EXTRACTOR_HPP
