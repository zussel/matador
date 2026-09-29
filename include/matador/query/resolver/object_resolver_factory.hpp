#ifndef MATADOR_OBJECT_RESOLVER_FACTORY_HPP
#define MATADOR_OBJECT_RESOLVER_FACTORY_HPP

#include "matador/query/resolver/abstract_type_resolver_factory.hpp"
#include "matador/query/object_resolver.hpp"

namespace matador::query {
class object_resolver_factory : public abstract_type_resolver_factory {
public:
  template<class Type>
  [[nodiscard]] std::shared_ptr<object_resolver<Type>> resolver() const {
    const auto res = acquire_object_resolver(std::type_index(typeid(Type)));
    if (!res) {
      return std::dynamic_pointer_cast<object_resolver<Type>>(res);
    }

    return std::dynamic_pointer_cast<object_resolver<Type>>(res);
  }
};
}
#endif //MATADOR_OBJECT_RESOLVER_FACTORY_HPP