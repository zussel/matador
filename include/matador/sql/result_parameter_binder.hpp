#ifndef QUERY_RESULT_PARAMETER_BINDER_HPP
#define QUERY_RESULT_PARAMETER_BINDER_HPP

#include "matador/utils/access.hpp"
#include "matador/utils/cascade_type.hpp"
#include "matador/utils/field_attributes.hpp"

#include "matador/object/data_type_traits.hpp"

#include <string>

namespace matador {

class time;
class date;

}
namespace matador::sql {

class result_parameter_binder;

namespace detail {
class fk_result_binder
{
public:
  explicit fk_result_binder(result_parameter_binder &result_binder);

  template<class Type>
  void bind_result(Type &obj, size_t column_index)
  {
    column_index_ = column_index;
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
  size_t column_index_{};
  result_parameter_binder &result_binder_;
};

}

class result_parameter_binder
{
public:
  template<typename ValueType>
  void on_primary_key(const char * /*id*/, ValueType &value, typename std::enable_if<std::is_integral<ValueType>::value && !std::is_same<bool, ValueType>::value>::type* = 0)
  {
    object::data_type_traits<ValueType>::bind_result_value(*this, column_index_++, value);
  }
  void on_primary_key(const char *id, std::string &value, size_t size);
  void on_revision(const char *id, unsigned long long &rev);

  template < class Type >
  void on_attribute(const char * /*id*/, Type &x, const utils::field_attributes &/*attr*/ = utils::null_attributes)
  {
    object::data_type_traits<Type>::bind_result_value(*this, column_index_++, x);
  }
  void on_attribute(const char *id, char *value, const utils::field_attributes &attr = utils::null_attributes);
  void on_attribute(const char *id, std::string &value, const utils::field_attributes &attr = utils::null_attributes);
  void on_attribute(const char *id, utils::any_type &value, data_type type, const utils::field_attributes &attr = utils::null_attributes);

  template < class Pointer >
  void on_belongs_to(const char * /*id*/, Pointer &x, utils::cascade_type)
  {
    if (!x.get()) {
      x.reset(new typename Pointer::value_type);
    }
    fk_result_binder_.bind_result(*x, column_index_++);
  }
  template < class Pointer >
  void on_has_one(const char * /*id*/, Pointer &x, utils::cascade_type)
  {
    if (!x.get()) {
      x.reset(new typename Pointer::value_type);
    }
    fk_result_binder_.bind_result(*x, column_index_++);
  }

  template<class ContainerType>
  void on_has_many(const char *, ContainerType &, const char *, const char *, utils::cascade_type) {}
  template<class ContainerType>
  void on_has_many(const char *, ContainerType &, utils::cascade_type) {}

  virtual void bind_result_value(size_t /*index*/, char &/*value*/) {}
  virtual void bind_result_value(size_t /*index*/, short &/*value*/) {}
  virtual void bind_result_value(size_t /*index*/, int &/*value*/) {}
  virtual void bind_result_value(size_t /*index*/, long &/*value*/) {}
  virtual void bind_result_value(size_t /*index*/, long long &/*value*/) {}
  virtual void bind_result_value(size_t /*index*/, unsigned char &/*value*/) {}
  virtual void bind_result_value(size_t /*index*/, unsigned short &/*value*/) {}
  virtual void bind_result_value(size_t /*index*/, unsigned int &/*value*/) {}
  virtual void bind_result_value(size_t /*index*/, unsigned long &/*value*/) {}
  virtual void bind_result_value(size_t /*index*/, unsigned long long &/*value*/) {}
  virtual void bind_result_value(size_t /*index*/, bool &/*value*/) {}
  virtual void bind_result_value(size_t /*index*/, float &/*value*/) {}
  virtual void bind_result_value(size_t /*index*/, double &/*value*/) {}
  virtual void bind_result_value(size_t /*index*/, matador::time &/*value*/) {}
  virtual void bind_result_value(size_t /*index*/, matador::date &/*value*/) {}
  virtual void bind_result_value(size_t /*index*/, char * /*value*/, size_t /*s*/) {}
  virtual void bind_result_value(size_t /*index*/, std::string &/*value*/) {}
  virtual void bind_result_value(size_t /*index*/, std::string &/*value*/, size_t /*s*/) {}
  virtual void bind_result_value(size_t /*index*/, utils::any_type &/*value*/, data_type /*type*/, size_t /*size*/) {}

private:
  size_t column_index_{};
  detail::fk_result_binder fk_result_binder_;
};

namespace detail {

template<typename ValueType>
void fk_result_binder::on_primary_key(const char * /*id*/, ValueType &value, typename std::enable_if<std::is_integral<ValueType>::value && !std::is_same<bool, ValueType>::value>::type *)
{
  object::data_type_traits<ValueType>::bind_result_value(result_binder_, column_index_++, value);
}


}
}

#endif //QUERY_RESULT_PARAMETER_BINDER_HPP
