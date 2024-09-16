#include "matador/sql/column_generator.hpp"

namespace matador::sql {

column_generator::column_generator(std::vector<column> &column_infos,
                                   const sql::schema &ts,
                                   const std::string &table_name,
                                   bool force_lazy)
: column_infos_(column_infos)
, table_schema_(ts)
, force_lazy_(force_lazy)
{
  table_name_stack_.push(table_name);
}

void column_generator::on_primary_key(const char *id, std::string &, size_t)
{
  push(id);
}

void column_generator::on_revision(const char *id, unsigned long long int &)
{
  push(id);
}

void column_generator::push(const std::string &column_name) const
{
  column_infos_.emplace_back(column_name);
}

}