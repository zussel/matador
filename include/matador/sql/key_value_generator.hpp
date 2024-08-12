#ifndef QUERY_KEY_VALUE_GENERATOR_HPP
#define QUERY_KEY_VALUE_GENERATOR_HPP

#include "matador/sql/fk_value_extractor.hpp"
#include "matador/sql/key_value_pair.hpp"

#include "matador/utils/foreign_attributes.hpp"

#include <vector>

namespace matador::sql {

class key_value_generator
{
private:
public:
  explicit key_value_generator(std::vector<key_value_pair> &result) : result_(result) {}

public:
  template < class Type >
  static std::vector<key_value_pair> generate(const Type &obj)
  {
    std::vector<key_value_pair> result;
    key_value_generator generator(result);
    access::process(generator, obj);

    return result;
  }

  template < class V >
  void on_primary_key(const char *id, V &x, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0)
  {
    result_.emplace_back(id, x);
  }
  void on_primary_key(const char *id, std::string &, size_t);
  void on_revision(const char *id, unsigned long long &/*rev*/);

  template<typename Type>
  void on_attribute(const char *id, Type &x, const utils::field_attributes &/*attr*/ = utils::null_attributes)
  {
    result_.emplace_back(id, x);
  }

  template<class Type, template < class ... > class Pointer>
  void on_belongs_to(const char *id, Pointer<Type> &x, const utils::foreign_attributes &/*attr*/)
  {
    result_.emplace_back(id, fk_value_extractor_.extract(*x));
  }
  template<class Type, template < class ... > class Pointer>
  void on_has_one(const char *id, Pointer<Type> &x, const utils::foreign_attributes &/*attr*/)
  {
    result_.emplace_back(id, fk_value_extractor_.extract(*x));
  }
  template<class ContainerType>
  void on_has_many(const char *, ContainerType &, const char *, const char *, const utils::foreign_attributes &/*attr*/) {}
  template<class ContainerType>
  void on_has_many(const char *, ContainerType &, const utils::foreign_attributes &/*attr*/) {}

private:
  detail::fk_value_extractor fk_value_extractor_;
  std::vector<key_value_pair> &result_;
};

}
#endif //QUERY_KEY_VALUE_GENERATOR_HPP
