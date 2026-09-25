#ifndef NOOP_BACKEND_SERVICE_HPP
#define NOOP_BACKEND_SERVICE_HPP

#include "matador/query/backend_provider.hpp"

#include <unordered_set>

namespace matador::test::orm {

class test_backend_service final : public query::backend_provider::basic_backend_service {
public:
  query::connection_impl *create(const query::connection_info &info) override;
  void destroy(query::connection_impl *impl) override;
  [[nodiscard]] const query::dialect *dialect() const override;

private:
  std::unordered_set<std::unique_ptr<query::connection_impl>> noop_connections_;
};

}

#endif //NOOP_BACKEND_SERVICE_HPP
