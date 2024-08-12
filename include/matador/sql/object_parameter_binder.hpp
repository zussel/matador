#ifndef QUERY_OBJECT_PARAMETER_BINDER_HPP
#define QUERY_OBJECT_PARAMETER_BINDER_HPP

#include "matador/object/attribute_binder.hpp"
#include "matador/object/data_type_traits.hpp"

#include "matador/utils/access.hpp"
#include "matador/utils/cascade_type.hpp"
#include "matador/utils/field_attributes.hpp"

#include <string>

namespace matador::sql {

namespace detail {
class fk_binder
{
public:
  explicit fk_binder(object::attribute_binder &binder);
  fk_binder(fk_binder &&x) = default;
  fk_binder& operator=(fk_binder &&x) noexcept;

  template<class Type>
  void bind(Type &obj, size_t column_index)
  {
    index_ = column_index;
    access::process(*this, obj);
  }

  template<typename ValueType>
  void on_primary_key(const char *id, ValueType &value, typename std::enable_if<std::is_integral<ValueType>::value && !std::is_same<bool, ValueType>::value>::type* = 0);
  void on_primary_key(const char *id, std::string &value, size_t size);
  void on_revision(const char * /*id*/, unsigned long long &/*rev*/) {}

  template < class Type >
  void on_attribute(const char * /*id*/, Type &/*x*/, const utils::field_attributes &/*attr*/ = utils::null_attributes) {}
  template < class Pointer >
  void on_belongs_to(const char * /*id*/, Pointer &/*x*/, utils::cascade_type) {}
  template < class Pointer >
  void on_has_one(const char * /*id*/, Pointer &/*x*/, utils::cascade_type) {}

  template<class ContainerType>
  void on_has_many(const char *, ContainerType &, const char *, const char *, utils::cascade_type) {}
  template<class ContainerType>
  void on_has_many(const char *, ContainerType &, utils::cascade_type) {}

private:
  object::attribute_binder &binder_;
  size_t index_{0};
};

}

class object_parameter_binder
{
public:
  explicit object_parameter_binder(object::attribute_binder &binder);
  object_parameter_binder(object_parameter_binder &&x) = default;
  object_parameter_binder& operator=(object_parameter_binder &&x) noexcept;

  void reset();

  template < class Type >
  void on_primary_key(const char * /*id*/, Type &val, typename std::enable_if<std::is_integral<Type>::value && !std::is_same<bool, Type>::value>::type* = 0)
  {
    object::data_type_traits<Type>::bind_value(binder_, index_++, val);
  }
  void on_primary_key(const char *id, std::string &, size_t size);
  void on_revision(const char *id, unsigned long long &/*rev*/);

  template<typename Type>
  void on_attribute(const char * /*id*/, Type &val, const utils::field_attributes &/*attr*/ = utils::null_attributes)
  {
    object::data_type_traits<Type>::bind_value(binder_, index_++, val);
  }

  template<class Type, template < class ... > class Pointer>
  void on_belongs_to(const char * /*id*/, Pointer<Type> &x, utils::cascade_type)
  {
    fk_binder_.bind(index_++, x);
  }
  template<class Type, template < class ... > class Pointer>
  void on_has_one(const char * /*id*/, Pointer<Type> &x, utils::cascade_type)
  {
    fk_binder_.bind(index_++, x);
  }
  template<class ContainerType>
  void on_has_many(const char *, ContainerType &, const char *, const char *, utils::cascade_type) {}
  template<class ContainerType>
  void on_has_many(const char *, ContainerType &, utils::cascade_type) {}

private:
  std::reference_wrapper<object::attribute_binder> binder_;
  size_t index_{0};
  detail::fk_binder fk_binder_;
};

namespace detail {

template<typename ValueType>
void fk_binder::on_primary_key(const char *id, ValueType &value, typename std::enable_if<std::is_integral<ValueType>::value && !std::is_same<bool, ValueType>::value>::type *)
{
  object::data_type_traits<ValueType>::bind_value(binder_, index_++, value);
}

}
}
#endif //QUERY_OBJECT_PARAMETER_BINDER_HPP
