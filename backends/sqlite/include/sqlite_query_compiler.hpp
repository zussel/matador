#ifndef SQLITE_QUERY_COMPILER_HPP
#define SQLITE_QUERY_COMPILER_HPP

#include "matador/sql/query_compiler.hpp"

namespace matador::backends::sqlite {

class sqlite_query_compiler final : public sql::query_compiler
{
public:
  explicit sqlite_query_compiler(const sql::dialect& d);

};

}

#endif //SQLITE_QUERY_COMPILER_HPP
