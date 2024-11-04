#ifndef MATADOR_OBJECT_RESULT_BINDER_HPP
#define MATADOR_OBJECT_RESULT_BINDER_HPP

#include "matador/utils/attribute_reader.hpp"
#include "matador/utils/default_type_traits.hpp"

#include "matador/utils/access.hpp"
#include "matador/utils/field_attributes.hpp"
#include "matador/utils/foreign_attributes.hpp"

namespace matador::sql {

namespace detail {

class fk_result_binder
{
public:
  template<class Type>
  void bind(Type &obj, const char *id, size_t column_index, utils::attribute_reader &binder)
  {
    binder_ = &binder;
    index_ = column_index;
    id_ = id;
    access::process(*this, obj);
    id_ = nullptr;
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
  utils::attribute_reader *binder_{};
  size_t index_{0};
  const char *id_{};
};

}

class object_result_binder {
public:
  template<class Type>
  void bind(Type &obj, utils::attribute_reader &binder) {
    binder_ = &binder;
    matador::access::process(*this, obj);
    binder_ = nullptr;
  }

  void reset();

  template < class Type >
  void on_primary_key(const char *id, Type &val, std::enable_if_t<std::is_integral_v<Type> && !std::is_same_v<bool, Type>>* = nullptr)
  {
    utils::data_type_traits<Type>::read_value(*binder_, id, index_++, val);
  }
  void on_primary_key(const char *id, std::string &, size_t size);
  void on_revision(const char *id, unsigned long long &/*rev*/);

  template<typename Type>
  void on_attribute(const char *id, Type &val, const utils::field_attributes &/*attr*/ = utils::null_attributes)
  {
    utils::data_type_traits<Type>::read_value(*binder_, id, index_++, val);
  }
  void on_attribute(const char *id, char *value, const utils::field_attributes &attr = utils::null_attributes);
  void on_attribute(const char *id, std::string &value, const utils::field_attributes &attr = utils::null_attributes);
  void on_attribute(const char *id, utils::value &val, const utils::field_attributes &attr = utils::null_attributes);

  template<class Type, template < class ... > class Pointer>
  void on_belongs_to(const char *id, Pointer<Type> &x, const utils::foreign_attributes &/*attr*/ = utils::default_foreign_attributes)
  {
    fk_result_binder_.bind(*x, id, index_++, *binder_);
  }
  template<class Type, template < class ... > class Pointer>
  void on_has_one(const char *id, Pointer<Type> &x, const utils::foreign_attributes &/*attr*/ = utils::default_foreign_attributes)
  {
    fk_result_binder_.bind(*x, id, index_++, *binder_);
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
  utils::attribute_reader *binder_{};
  size_t index_{0};
  detail::fk_result_binder fk_result_binder_;
};

namespace detail {

template<typename ValueType>
void fk_result_binder::on_primary_key(const char * /*id*/, ValueType &value, std::enable_if_t<std::is_integral_v<ValueType> && !std::is_same_v<bool, ValueType>> *)
{
  utils::data_type_traits<ValueType>::read_value(*binder_, id_, index_++, value);
}

}

}

#endif //MATADOR_OBJECT_RESULT_BINDER_HPP
