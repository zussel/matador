#ifndef MATADOR_QUERY_CONTEXTS_HPP
#define MATADOR_QUERY_CONTEXTS_HPP

#include "matador/query/query_context.hpp"

namespace matador::query {
struct query_contexts {
  query_context insert;
  query_context update_one;
  query_context delete_one;
  query_context select_one;
  query_context select_all;
};
}
#endif //MATADOR_QUERY_CONTEXTS_HPP
