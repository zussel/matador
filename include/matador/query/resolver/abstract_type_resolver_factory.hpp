#ifndef MATADOR_ABSTRACT_TYPE_RESOLVER_FACTORY_HPP
#define MATADOR_ABSTRACT_TYPE_RESOLVER_FACTORY_HPP

#include "matador/query/resolver/abstract_resolver.hpp"

#include <memory>

namespace matador::query {
class abstract_type_resolver_factory {
public:
  virtual ~abstract_type_resolver_factory() = default;

  [[nodiscard]] virtual std::shared_ptr<abstract_resolver> acquire_object_resolver(const std::type_index &type) const = 0;
  virtual void register_object_resolver(std::shared_ptr<abstract_resolver> &&resolver) = 0;
};
}
#endif //MATADOR_ABSTRACT_TYPE_RESOLVER_FACTORY_HPP