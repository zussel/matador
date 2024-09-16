#include <utility>

#include "matador/sql/column.hpp"
#include "matador/sql/table.hpp"

namespace matador::sql {

column operator ""_col(const char *name, size_t len)
{
  return column{{name, len}};
}

column::column(const char *name, const std::string& as)
: column(std::string(name), as)
{}

column::column(std::string name, std::string as)
: table_(std::make_shared<sql::table>())
, name(std::move(name))
, alias(std::move(as)) {}

column::column(const sql_function_t func, std::string name)
: table_(std::make_shared<sql::table>())
, name(std::move(name))
, function_(func) {}

column::column(const struct table &t, const char *name, std::string as)
: column(t, std::string(name), std::move(as))
{}

column::column(const struct sql::table& t, std::string name, std::string as)
: table_(std::make_shared<sql::table>(t))
, name(std::move(name))
, alias(std::move(as)) {
    table_->columns.push_back(*this);
}

column::column(const std::shared_ptr<table>& t, std::string name, std::string as)
: table_(t)
, name(std::move(name))
, alias(std::move(as)) {
}

bool column::equals(const column &x) const
{
  return *table_ == *x.table_ &&
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