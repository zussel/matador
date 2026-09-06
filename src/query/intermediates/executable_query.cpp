#include "matador/query/intermediates/executable_query.hpp"
#include "matador/query/query_builder.hpp"

#include "matador/query/execute_result.hpp"
#include "matador/query/executor.hpp"
//#include "matador/sql/statement.hpp"

namespace matador::query {

utils::result<execute_result, utils::error> executable_query::execute(const executor &exec) const {
  query_builder compiler;
  return exec.execute(compiler.build(*context_, exec.dialect(), std::nullopt));
}

// utils::result<statement, utils::error> executable_query::prepare(executor &exec) const {
  // query_builder compiler;
  // return exec.prepare(compiler.build(*context_, exec.dialect(), std::nullopt));
// }

query_context executable_query::compile(const dialect& d) const {
  query_builder compiler;
  return compiler.build(*context_, d, std::nullopt);
}

std::string executable_query::str(const dialect& d) const {
  return compile(d).sql;
}

}
