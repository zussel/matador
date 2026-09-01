#ifndef MATADOR_COLLECTION_RESOLVER_HPP
#define MATADOR_COLLECTION_RESOLVER_HPP

#include "matador/query/abstract_joined_resolver.hpp"
#include "matador/query/object_ptr.hpp"

#include <any>
#include <typeindex>
#include <vector>

namespace matador::query {
class identifier;

template<typename Type>
class collection_resolver : public abstract_joined_resolver {
public:
  using value_type = Type;

  collection_resolver(const std::type_index& root_type, std::string collection_name)
  : abstract_joined_resolver(root_type, typeid(Type), std::move(collection_name)) {}

  virtual std::vector<value_type> resolve(const identifier& id) = 0;

protected:
  std::any owner_;
};
}
#endif //MATADOR_COLLECTION_RESOLVER_HPP