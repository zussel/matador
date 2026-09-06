#ifndef EXECUTABLE_QUERY_HPP
#define EXECUTABLE_QUERY_HPP

#include "matador/query/intermediates/intermediate.hpp"

#include "matador/utils/error.hpp"
#include "matador/utils/result.hpp"

namespace matador::query {
class dialect;
class executor;
class statement;
struct execute_result;
struct query_context;

class executable_query : public intermediate {
public:
  using intermediate::intermediate;

  [[nodiscard]] result<execute_result, error> execute(const executor &exec) const;
  // [[nodiscard]] result<statement, error> prepare(executor &exec) const;
  [[nodiscard]] query_context compile(const dialect &d) const;
  [[nodiscard]] std::string str(const dialect &d) const;
};

}

#endif //EXECUTABLE_QUERY_HPP
