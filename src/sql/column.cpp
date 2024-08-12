#include "matador/sql/column.hpp"
#include "matador/sql/table.hpp"

namespace matador::sql {

column operator ""_col(const char *name, size_t len)
{
  return {{name, len}};
}

column::column(const char *name) : name(name) {}

column::column(std::string name) : name(std::move(name)) {}

column::column(sql_function_t func, std::string name) : name(std::move(name)), function_(func) {}

column::column(std::string table_name, std::string name, std::string as)
: table(std::move(table_name))
, name(std::move(name))
, alias(std::move(as)) {}

column::column(std::string table_name, const char *name, std::string as)
: table(std::move(table_name))
, name(name)
, alias(std::move(as)) {}

column::column(struct table &t, const char *name, std::string as)
: table(t.name)
, name(name)
, alias(std::move(as))
{
  t.columns.push_back(*this);
}

bool column::equals(const column &x) const
{
  return table == x.table &&
         name == x.name &&
         alias == x.alias &&
         function_ == x.function_;
}

column &column::as(std::string a)
{
  alias = std::move(a);
  return *this;
}

bool column::is_function() const
{
  return function_ != sql_function_t::NONE;
}

bool column::has_alias() const {
    return !alias.empty();
}

}