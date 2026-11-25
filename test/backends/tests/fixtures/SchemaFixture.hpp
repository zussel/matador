#ifndef MATADOR_QUERY_SCHEMA_FIXTURE_HPP
#define MATADOR_QUERY_SCHEMA_FIXTURE_HPP

#include "matador/query/runtime/connection_pool.hpp"

namespace matador::test {
class SchemaFixture {
public:
  SchemaFixture();

protected:
  query::connection_pool pool;
};
}

#endif //MATADOR_QUERY_SCHEMA_FIXTURE_HPP