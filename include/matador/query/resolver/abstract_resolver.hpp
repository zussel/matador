#ifndef MATADOR_ABSTRACT_TYPE_RESOLVER_HPP
#define MATADOR_ABSTRACT_TYPE_RESOLVER_HPP

#include <typeindex>

namespace matador::query {
class abstract_resolver {
public:
  virtual ~abstract_resolver() = default;

  [[nodiscard]] const std::type_index& type() const { return type_; }

protected:
  explicit abstract_resolver(const std::type_index& ti) : type_(ti) {}

public:
  const std::type_index type_;
};
}
#endif //MATADOR_ABSTRACT_TYPE_RESOLVER_HPP