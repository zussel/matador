#ifndef QUERY_INTO_INTERMEDIATE_HPP
#define QUERY_INTO_INTERMEDIATE_HPP

#include "matador/query/intermediates/values_intermediate.hpp"
#include "matador/query/internal/value_extractor.hpp"
#include "matador/query/object_ptr.hpp"
#include "matador/query/placeholder.hpp"

namespace matador::query {
class into_intermediate : public intermediate {
public:
  using intermediate::intermediate;

  values_intermediate values(std::initializer_list<std::variant<placeholder, utils::database_type> > values) const;
  values_intermediate values(std::vector<std::variant<placeholder, utils::database_type> > &&values) const;
  values_intermediate values(std::vector<placeholder> &&values) const;
  values_intermediate values(std::vector<utils::database_type> &&values) const;

  template<class Type>
  values_intermediate values(const Type &obj) const {
    return values(value_extractor::extract(obj));
  }

  template<class Type>
  values_intermediate values(const object_ptr<Type> &obj) const {
    return values(*obj);
  }
};
}

#endif //QUERY_INTO_INTERMEDIATE_HPP
