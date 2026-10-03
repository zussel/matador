#ifndef MATADOR_OBJECT_RESOLVER_HPP
#define MATADOR_OBJECT_RESOLVER_HPP

#include "matador/query/resolver/abstract_resolver.hpp"
#include "matador/query/resolver/abstract_joined_resolver.hpp"

#include <memory>

namespace matador::query {
class identifier;
template<typename Type>
class object_resolver : public abstract_resolver {
public:
  object_resolver() : abstract_resolver(typeid(Type)) {}

  virtual std::shared_ptr<Type> resolve(const identifier& id) = 0;
};

template<typename Type>
class joined_object_resolver : public abstract_joined_resolver, public object_resolver<Type> {
public:
  joined_object_resolver(const std::type_index& root_type, const std::string& join_column)
  : abstract_joined_resolver(root_type, typeid(Type), join_column) {}

  std::shared_ptr<Type> resolve(const identifier& id) override = 0;
};

}
#endif //MATADOR_OBJECT_RESOLVER_HPP
