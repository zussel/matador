#ifndef QUERY_FIXTURE_HPP
#define QUERY_FIXTURE_HPP

#include "matador/query/runtime/connection.hpp"

#include "matador/query/internal/query_contexts.hpp"

#include <memory>

namespace matador::query {
class basic_schema;
}

namespace matador::test {

class QueryFixture {
public:
  QueryFixture();
  ~QueryFixture() = default;

protected:
  static std::unordered_map<std::type_index, query::query_contexts> to_contexts_by_name(const query::basic_schema& scm, const query::dialect& d) ;

protected:
  std::unique_ptr<query::connection> db;
};

}

#endif //QUERY_FIXTURE_HPP
