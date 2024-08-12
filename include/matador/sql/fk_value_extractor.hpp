#ifndef QUERY_FK_VALUE_EXTRACTOR_HPP
#define QUERY_FK_VALUE_EXTRACTOR_HPP

#include "matador/utils/access.hpp"
#include "matador/utils/field_attributes.hpp"
#include "matador/utils/foreign_attributes.hpp"
#include "matador/utils/types.hpp"

namespace matador::sql::detail {

class fk_value_extractor
{
public:
  fk_value_extractor() = default;

  template<class Type>
  utils::any_type extract(Type &x)
  {
    matador::access::process(*this, x);
    return value_;
  }

  template<typename ValueType>
  void on_primary_key(const char *, ValueType &pk, typename std::enable_if<std::is_integral<ValueType>::value && !std::is_same<bool, ValueType>::value>::type* = 0)
  {
    value_ = pk;
  }
  void on_primary_key(const char * /*id*/, std::string &pk, size_t size);
  void on_revision(const char * /*id*/, unsigned long long &/*rev*/) {}
  template < class Type >
  void on_attribute(const char * /*id*/, Type &/*x*/, const utils::field_attributes &/*attr*/ = utils::null_attributes) {}
  void on_attribute(const char * /*id*/, char * /*x*/, const utils::field_attributes &/*attr*/ = utils::null_attributes) {}
  template<class Pointer>
  void on_belongs_to(const char * /*id*/, Pointer &/*x*/, const utils::foreign_attributes &/*attr*/) {}
  template<class Pointer>
  void on_has_one(const char * /*id*/, Pointer &/*x*/, const utils::foreign_attributes &/*attr*/) {}
  template<class ContainerType>
  void on_has_many(const char *, ContainerType &, const char *, const char *, const utils::foreign_attributes &/*attr*/) {}
  template<class ContainerType>
  void on_has_many(const char *, ContainerType &, const utils::foreign_attributes &/*attr*/) {}


private:
  utils::any_type value_{};
};

}

#endif //QUERY_FK_VALUE_EXTRACTOR_HPP
