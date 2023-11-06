#ifndef MATADOR_STATEMENT_CONTEXT_HPP
#define MATADOR_STATEMENT_CONTEXT_HPP

#include <string>
#include <utility>
#include <vector>

namespace matador {
namespace detail {

/// @cond MATADOR_DEV

struct statement_context
{
  std::string sql;
  std::vector<std::string> bind_vars;
  std::vector<std::string> columns;
};

/// @endcond

}
}

#endif //MATADOR_STATEMENT_CONTEXT_HPP
