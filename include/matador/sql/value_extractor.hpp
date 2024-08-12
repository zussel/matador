#ifndef QUERY_VALUE_EXTRACTOR_HPP
#define QUERY_VALUE_EXTRACTOR_HPP

#include "matador/sql/fk_value_extractor.hpp"

#include "matador/object/attribute_binder.hpp"
#include "matador/object/data_type_traits.hpp"

#include "matador/utils/foreign_attributes.hpp"

#include <vector>

namespace matador::sql {

class value_extractor : public object::attribute_binder
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
  void on_primary_key(const char *, ValueType &x, typename std::enable_if<std::is_integral<ValueType>::value && !std::is_same<bool, ValueType>::value>::type* = 0)
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
  void bind(size_t pos, char i) override;
  void bind(size_t pos, short i) override;
  void bind(size_t pos, int i) override;
  void bind(size_t pos, long i) override;
  void bind(size_t pos, long long int i) override;
  void bind(size_t pos, unsigned char i) override;
  void bind(size_t pos, unsigned short i) override;
  void bind(size_t pos, unsigned int i) override;
  void bind(size_t pos, unsigned long i) override;
  void bind(size_t pos, unsigned long long int i) override;
  void bind(size_t pos, bool b) override;
  void bind(size_t pos, float d) override;
  void bind(size_t pos, double d) override;
  void bind(size_t pos, const time &time) override;
  void bind(size_t pos, const date &date) override;
  void bind(size_t pos, const char *string) override;
  void bind(size_t pos, const char *string, size_t size) override;
  void bind(size_t pos, const std::string &string) override;
  void bind(size_t pos, const std::string &x, size_t size) override;
  void bind(size_t pos, const utils::blob &blob) override;

private:
  detail::fk_value_extractor fk_value_extractor_;
  std::vector<utils::any_type> &values_;
};

}

#endif //QUERY_VALUE_EXTRACTOR_HPP
