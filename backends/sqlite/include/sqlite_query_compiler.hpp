#ifndef SQLITE_QUERY_COMPILER_HPP
#define SQLITE_QUERY_COMPILER_HPP

#include "matador/sql/query_compiler.hpp"

namespace matador::backends::sqlite {

class sqlite_query_compiler final : public sql::query_compiler
{
protected:
  void visit(sql::query_select_part &select_part) override;
  void visit(sql::query_update_part &update_part) override;
  void visit(sql::query_delete_part &delete_part) override;
  void visit(sql::query_where_part &where_part) override;

  void visit(sql::query_limit_part &limit_part) override;

private:
  bool is_update = false;
  bool is_delete = false;
};

}

#endif //SQLITE_QUERY_COMPILER_HPP
