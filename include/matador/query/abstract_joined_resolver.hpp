#ifndef MATADOR_ABSTRACT_COLLECTION_RESOLVER_HPP
#define MATADOR_ABSTRACT_COLLECTION_RESOLVER_HPP

#include "matador/query/abstract_type_resolver.hpp"

#include <string>
#include <typeindex>

namespace matador::query {
class abstract_joined_resolver : public abstract_type_resolver {
public:
  [[nodiscard]] const std::type_index& root_type() const;
  [[nodiscard]] const std::string& collection_name() const;

protected:
  explicit abstract_joined_resolver(const std::type_index& root_type, const std::type_index& type, std::string collection_name);

public:
  const std::type_index root_type_;
  const std::string collection_name_;
};
}
#endif  // MATADOR_ABSTRACT_COLLECTION_RESOLVER_HPP
