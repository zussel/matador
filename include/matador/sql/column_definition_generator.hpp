#ifndef QUERY_COLUMN_DEFINITION_GENERATOR_HPP
#define QUERY_COLUMN_DEFINITION_GENERATOR_HPP

#include "matador/sql/column_definition.hpp"
#include "matador/utils/default_type_traits.hpp"

#include "matador/utils/access.hpp"
#include "matador/utils/field_attributes.hpp"
#include "matador/utils/foreign_attributes.hpp"

#include <typeindex>
#include <vector>

namespace matador::sql {

class schema;

class fk_column_generator
{
public:
  fk_column_generator() = default;

  template<class Type>
  column_definition generate(const char *id, Type &x, const std::string &ref_table, const std::string &ref_column)
  {
    access::process(*this, x);
    return column_definition{id, type_, 0, ref_table, ref_column, {utils::constraints::FOREIGN_KEY }, null_option::NOT_NULL};
  }

  template<typename ValueType>
  void on_primary_key(const char *, ValueType &/*pk*/, typename std::enable_if<std::is_integral<ValueType>::value && !std::is_same<bool, ValueType>::value>::type* = 0)
  {
    type_ = utils::data_type_traits<ValueType>::type(0);
  }
  void on_primary_key(const char * /*id*/, std::string &/*pk*/, size_t size);
  void on_revision(const char * /*id*/, unsigned long long &/*rev*/) {}
  template < class Type >
  void on_attribute(const char * /*id*/, Type &/*x*/, const utils::field_attributes &/*attr*/ = utils::null_attributes) {}
  void on_attribute(const char * /*id*/, char * /*x*/, const utils::field_attributes &/*attr*/ = utils::null_attributes) {}
  template<class Pointer>
  void on_belongs_to(const char * /*id*/, Pointer &/*x*/, const utils::foreign_attributes &/*attr*/) {}
  template<class Pointer>
  void on_has_one(const char * /*id*/, Pointer &/*x*/, const utils::foreign_attributes &/*attr*/) {}
  template<class ContainerType>
  void on_has_many(const char * /*id*/, ContainerType &/*c*/, const char *, const utils::foreign_attributes &/*attr*/) {}
  template<class ContainerType>
  void on_has_many_to_many(const char * /*id*/,
                           ContainerType &/*c*/,
                           const char * /*join_column*/,
                           const char * /*inverse_join_column*/,
                           const utils::foreign_attributes &/*attr*/) {}
  template<class ContainerType>
  void on_has_many_to_many(const char * /*id*/, ContainerType &/*c*/, const utils::foreign_attributes &/*attr*/) {}

private:
  data_type type_{};
};

class column_definition_generator
{
private:
  column_definition_generator(std::vector<column_definition> &columns, const schema &repo);

public:
  ~column_definition_generator() = default;

  template < class Type >
  static std::vector<column_definition> generate(const schema &repo)
  {
    std::vector<column_definition> columns;
    column_definition_generator gen(columns, repo);
    Type obj;
    access::process(gen, obj);
    return columns;
  }

  template < class V >
  void on_primary_key(const char *, V &x, std::enable_if_t<std::is_integral_v<V> && !std::is_same_v<bool, V>>* = nullptr);
  void on_primary_key(const char *id, std::string &pk, size_t size);
  void on_revision(const char *id, unsigned long long &rev);

  template<typename Type>
  void on_attribute(const char *id, Type &x, const utils::field_attributes &attr = utils::null_attributes);

  template<typename Type>
  void on_attribute(const char *id, std::optional<Type> &x, const utils::field_attributes &attr = utils::null_attributes);

  void on_attribute(const char *id, const char *x, const utils::field_attributes &attr);
  void on_attribute(const char *id, std::optional<const char*> x, const utils::field_attributes &attr);

  template<class Pointer>
  void on_belongs_to(const char *id, Pointer &x, const utils::foreign_attributes &/*attr*/)
  {
    const auto [ref_table, ref_column] = determine_foreign_ref(std::type_index(typeid(typename Pointer::value_type)));
    columns_.push_back(fk_column_generator_.generate(id, *x, ref_table, ref_column));
  }
  template<class Pointer>
  void on_has_one(const char *id, Pointer &x, const utils::foreign_attributes &/*attr*/)
  {
    const auto [ref_table, ref_column] = determine_foreign_ref(std::type_index(typeid(typename Pointer::value_type)));
    columns_.push_back(fk_column_generator_.generate(id, *x, ref_table, ref_column));
  }
  template<class ContainerType>
  void on_has_many(const char * /*id*/,
                   ContainerType &/*c*/,
                   const char * /*join_column*/,
                   const utils::foreign_attributes &/*attr*/) {}
  template<class ContainerType>
  void on_has_many_to_many(const char * /*id*/,
                           ContainerType & /*c*/,
                           const char * /*join_column*/,
                           const char * /*inverse_join_column*/,
                           const utils::foreign_attributes &/*attr*/) {}
  template<class ContainerType>
  void on_has_many_to_many(const char * /*id*/,
                           ContainerType &/*c*/,
                           const utils::foreign_attributes &/*attr*/) {}

private:
  std::pair<std::string, std::string> determine_foreign_ref(const std::type_index &ti);

private:
  size_t index_ = 0;
  std::vector<column_definition> &columns_;
  const schema &repo_;

  fk_column_generator fk_column_generator_;
};

template<typename V>
void column_definition_generator::on_primary_key(const char *id, V &x, std::enable_if_t<std::is_integral_v<V> && !std::is_same_v<bool, V>>*)
{
  on_attribute(id, x, { utils::constraints::PRIMARY_KEY });
}

template<typename Type>
void column_definition_generator::on_attribute(const char *id, Type &x, const utils::field_attributes &attr)
{
  columns_.emplace_back(id, x, attr, null_option::NOT_NULL);
}

template<typename Type>
void column_definition_generator::on_attribute(const char *id, std::optional<Type> &/*x*/, const utils::field_attributes &attr)
{
  columns_.emplace_back(id, utils::data_type_traits<Type>::type(attr.size()), attr, null_option::NULLABLE);
}

}
#endif //QUERY_COLUMN_DEFINITION_GENERATOR_HPP
