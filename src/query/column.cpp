#include "matador/query/column.hpp"

#include "matador/query/table.hpp"

#include <stdexcept>
#include <utility>

// #include "matador/query/expression/column_expression.hpp"

namespace matador::query {

column operator ""_col(const char *name, const size_t len) {
  const std::string str(name, len);
  const auto pos = str.find('.');
  if (pos == std::string::npos) {
    return column{str};
  }

  if (str.find('.', pos + 1) != std::string::npos) {
    throw std::invalid_argument("Invalid column name: multiple dots found");
  }

  return column{str.substr(pos + 1)};
}

column::column(const char *name)
: column(std::string(name))
{}

column::column(const std::string& name)
: column(name, "") {}

column::column(const std::string& name, const std::string& alias)
: column(nullptr, name, alias, utils::basic_type::Unknown, query_functions::None, {})
{}

column::column(const query_functions func, const std::string &name)
: column(nullptr, name, "", utils::basic_type::Unknown, func, {})
{}

column::column(const class table* tab, const std::string& name)
: column(tab, name, "", utils::basic_type::Unknown, query_functions::None, {})
{}

column::column(const class table* tab, const std::string& name, const std::string& alias)
: column(tab, name, alias, utils::basic_type::Unknown, query_functions::None, {})
{}

column::column(const class table* tab, const std::string& name, const utils::basic_type type)
: column(tab, name, "", type, query_functions::None, {})
{}

column::column(const std::shared_ptr<abstract_column_expression> &expression)
: column(nullptr, "", "", utils::basic_type::Unknown, query_functions::None, expression)
{}

column::column(const class table *tab,
                           std::string name,
                           std::string alias,
                           const utils::basic_type type,
                           const query_functions func,
                           const std::shared_ptr<abstract_column_expression>& expression)
: table_(tab)
, column_name_(std::move(name))
, canonical_name_(build_canonical_name(table_, name))
, alias_(std::move(alias))
, type_(type)
, function_(func)
, expression_(expression){}

column & column::operator=(const column &other) {
  if (this == &other) {
    return *this;
  }
  table_ = other.table_;
  column_name_ = other.column_name_;
  canonical_name_ = other.canonical_name_;
  alias_ = other.alias_;
  type_ = other.type_;
  function_ = other.function_;
  expression_ = other.expression_;
  return *this;
}

bool column::equals(const column &x) const {
  if (table_ != nullptr && x.table_ != nullptr) {
    return *table_ == *x.table_ &&
           column_name_ == x.column_name_ &&
           canonical_name_ == x.canonical_name_ &&
           alias_ == x.alias_;
  }

  return alias_ == x.alias_;
}

column column::as(const std::string& alias) const {
  return {table_, column_name_, alias, type_, function_, expression_};
}

const std::string& column::name() const {
  return canonical_name_;
}

const std::string& column::column_name() const {
  return column_name_;
}

const std::string& column::canonical_name() const {
  return canonical_name_;
}

const std::string& column::alias() const {
  return alias_;
}
const std::string& column::result_name() const {
  return alias_.empty() ? column_name_ : alias_;
}
utils::basic_type column::type() const {
  return type_;
}

bool column::is_function() const {
  return function_ != query_functions::None;
}

bool column::is_expression() const {
  return static_cast<bool>(expression_);
}

// bool query_column::is_nullable() const {
//   return !utils::is_constraint_set(attributes_.options(), utils::constraints::NotNull);
// }
//
// bool query_column::is_primary_key() const {
//   return utils::is_constraint_set(attributes_.options(), utils::constraints::PrimaryKey);
// }
//
// bool query_column::is_foreign_key() const {
//   return utils::is_constraint_set(attributes_.options(), utils::constraints::ForeignKey);
// }
//
// bool query_column::is_unique() const {
//   return utils::is_constraint_set(attributes_.options(), utils::constraints::Unique);
// }
//
// bool query_column::is_identity() const {
//   return utils::is_constraint_set(attributes_.options(), utils::constraints::Identity);
// }

query_functions column::function() const {
  return function_;
}

bool column::has_alias() const {
  return !alias_.empty();
}

const table* column::table() const {
  return table_;
}

void column::table(const class table* tab) {
  table_ = tab;
  canonical_name_ = build_canonical_name(table_, column_name_);
}

column::operator const std::string&() const {
  return name();
}

std::shared_ptr<abstract_column_expression> column::expression() const {
  return expression_;
}

std::string column::build_canonical_name(const class table* tab, const std::string& name) {
  return tab != nullptr ? tab->name() + "." + name : name;
}
}  // namespace matador::query
