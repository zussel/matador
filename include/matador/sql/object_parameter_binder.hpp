#ifndef QUERY_OBJECT_PARAMETER_BINDER_HPP
#define QUERY_OBJECT_PARAMETER_BINDER_HPP

#include "matador/utils/attribute_writer.hpp"
#include "matador/utils/default_type_traits.hpp"

#include "matador/utils/access.hpp"
#include "matador/utils/field_attributes.hpp"
#include "matador/utils/foreign_attributes.hpp"

#include <string>

namespace matador::sql {

namespace detail {

class fk_binder
{
public:
  template<class Type>
  void bind(Type &obj, size_t column_index, utils::attribute_writer &binder)
  {
    binder_ = &binder;
    index_ = column_index;
    access::process(*this, obj);
    binder_ = nullptr;
  }

  template<typename ValueType>
  void on_primary_key(const char *id, ValueType &value, std::enable_if_t<std::is_integral_v<ValueType> && !std::is_same_v<bool, ValueType>>* = nullptr);
  void on_primary_key(const char *id, std::string &value, size_t size);
  void on_revision(const char * /*id*/, unsigned long long &/*rev*/) {}

  template < class Type >
  void on_attribute(const char * /*id*/, Type &/*x*/, const utils::field_attributes &/*attr*/ = utils::null_attributes) {}
  template < class Pointer >
  void on_belongs_to(const char * /*id*/, Pointer &/*x*/, const utils::foreign_attributes &/*attr*/ = utils::default_foreign_attributes) {}
  template < class Pointer >
  void on_has_one(const char * /*id*/, Pointer &/*x*/, const utils::foreign_attributes &/*attr*/ = utils::default_foreign_attributes) {}

  template<class ContainerType>
  void on_has_many(const char * /*id*/,
                   ContainerType &/*c*/,
                   const char * /*join_column*/,
                   const utils::foreign_attributes &/*attr*/ = utils::default_foreign_attributes) {}
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

private:
  utils::attribute_writer *binder_{};
  size_t index_{0};
};

}

class object_parameter_binder
{
public:
  template<class Type>
  void bind(Type &obj, utils::attribute_writer &binder) {
    binder_ = &binder;
    access::process(*this, obj);
    binder_ = nullptr;
  }

  void reset();

  template < class Type >
  void on_primary_key(const char * /*id*/, Type &val, std::enable_if_t<std::is_integral_v<Type> && !std::is_same_v<bool, Type>>* = nullptr)
  {
    utils::data_type_traits<Type>::bind_value(*binder_, index_++, val);
  }
  void on_primary_key(const char *id, std::string &, size_t size);
  void on_revision(const char *id, unsigned long long &/*rev*/);

  template<typename Type>
  void on_attribute(const char * /*id*/, Type &val, const utils::field_attributes &/*attr*/ = utils::null_attributes)
  {
    utils::data_type_traits<Type>::bind_value(*binder_, index_++, val);
  }

  template<class Type, template < class ... > class Pointer>
  void on_belongs_to(const char * /*id*/, Pointer<Type> &x, const utils::foreign_attributes &/*attr*/ = utils::default_foreign_attributes)
  {
    fk_binder_.bind(*x, index_++, *binder_);
  }
  template<class Type, template < class ... > class Pointer>
  void on_has_one(const char * /*id*/, Pointer<Type> &x, const utils::foreign_attributes &/*attr*/ = utils::default_foreign_attributes)
  {
    fk_binder_.bind(*x, index_++, *binder_);
  }
  template<class ContainerType>
  void on_has_many(const char * /*id*/,
                   ContainerType &/*c*/,
                   const char * /*join_column*/,
                   const utils::foreign_attributes &/*attr*/ = utils::default_foreign_attributes) {}
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

private:
  utils::attribute_writer *binder_{};
  size_t index_{0};
  detail::fk_binder fk_binder_;
};

namespace detail {

template<typename ValueType>
void fk_binder::on_primary_key(const char * /*id*/, ValueType &value, std::enable_if_t<std::is_integral_v<ValueType> && !std::is_same_v<bool, ValueType>> *)
{
  utils::data_type_traits<ValueType>::bind_value(*binder_, index_++, value);
}

}
}
#endif //QUERY_OBJECT_PARAMETER_BINDER_HPP
