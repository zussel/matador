#include "matador/query/observer.hpp"

namespace matador::query {
abstract_observer::abstract_observer(const std::type_index& ti)
: type_index_(ti) {}

const std::type_index& abstract_observer::index() const {
  return type_index_;
}
}  // namespace matador::object