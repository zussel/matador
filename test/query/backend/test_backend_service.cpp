#include "test_backend_service.hpp"
#include "test_connection.hpp"

#include "matador/query/dialect_builder.hpp"

#include <algorithm>

namespace matador::test::orm {

query::connection_impl *test_backend_service::create(const query::connection_info &info) {
  return noop_connections_.insert(std::make_unique<test_connection>(info)).first->get();
}

void test_backend_service::destroy(query::connection_impl *impl) {
  const auto it = std::find_if(noop_connections_.begin(), noop_connections_.end(), [impl](const auto &item) {
    return impl == item.get();
  });
  if (it != noop_connections_.end()) {
    noop_connections_.erase(it);
  }
}

const query::dialect *test_backend_service::dialect() const {
  static query::dialect dialect_ = query::dialect_builder::builder()
    .create()
    .build();
  return &dialect_;
}

}