#include "QueryFixture.hpp"

#include "../backend/test_backend_service.hpp"

#include "matador/query/interface/connection_impl.hpp"

#include "matador/query/schema/basic_schema.hpp"
#include "matador/query/internal/schema_utils.hpp"

namespace matador::test {

QueryFixture::QueryFixture() {
  query::backend_provider::instance().register_backend("noop", std::make_unique<orm::test_backend_service>());

  db = std::make_unique<query::connection>("noop://noop.db");
}

std::unordered_map<std::type_index, query::query_contexts> QueryFixture::to_contexts_by_name(const query::basic_schema& scm, const query::dialect& d) {
  std::unordered_map<std::type_index, query::query_contexts> contexts_by_type;
  for (const auto &node : scm) {
    contexts_by_type[node.type_index()] = to_query_contexts(node, d);
  }

    return contexts_by_type;
}
}