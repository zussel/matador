#ifndef QUERY_PLACEHOLDER_GENERATOR_HPP
#define QUERY_PLACEHOLDER_GENERATOR_HPP

#include "matador/utils/cascade_type.hpp"
#include "matador/utils/field_attributes.hpp"
#include "matador/utils/types.hpp"

#include <vector>

namespace matador::sql {

class placeholder_generator
{
public:
  template < class V >
  void on_primary_key(const char * /*id*/, V &/*val*/, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0)
  {
    placeholder_values.emplace_back(utils::_);
  }
  void on_primary_key(const char *id, std::string &, size_t);
  void on_revision(const char *id, unsigned long long &/*rev*/);

  template<typename Type>
  void on_attribute(const char * /*id*/, Type &/*val*/, const utils::field_attributes &/*attr*/ = utils::null_attributes)
  {
    placeholder_values.emplace_back(utils::_);
  }

  template<class Type, template < class ... > class Pointer>
  void on_belongs_to(const char * /*id*/, Pointer<Type> &/*x*/, utils::cascade_type)
  {
    placeholder_values.emplace_back(utils::_);
  }
  template<class Type, template < class ... > class Pointer>
  void on_has_one(const char * /*id*/, Pointer<Type> &/*x*/, utils::cascade_type)
  {
    placeholder_values.emplace_back(utils::_);
  }
  template<class ContainerType>
  void on_has_many(const char *, ContainerType &, const char *, const char *, utils::cascade_type) {}
  template<class ContainerType>
  void on_has_many(const char *, ContainerType &, utils::cascade_type) {}

  std::vector<utils::any_type> placeholder_values;
};

}
#endif //QUERY_PLACEHOLDER_GENERATOR_HPP
