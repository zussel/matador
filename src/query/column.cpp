#include "matador/query/column.hpp"

#include "matador/query/table.hpp"

#include <stdexcept>
#include <utility>

// #include "matador/query/expression/column_expression.hpp"

namespace matador::query {

column operator ""_col(const char *name, const size_t len) {
  const std::string str(name, len);
  if (str.find('.') != std::string::npos) {
    throw std::invalid_argument("Qualified column literals are not supported");
  }

  return column::make_plain(str);
}

column column::make_plain(
  std::string name,
  std::string alias,
  const utils::basic_type type,
  const column_options options
) {
  return column{
    nullptr,
    std::move(name),
    std::move(alias),
    type,
    query_functions::None,
    {},
    options,
    0
  };
}

column column::make_plain(
  const class table* tab,
  std::string name,
  std::string alias,
  const utils::basic_type type,
  const column_options options,
  const size_t index
) {
  return column{
    tab,
    std::move(name),
    std::move(alias),
    type,
    query_functions::None,
    {},
    options,
    index
  };
}

column column::make_query_function(
  const query_functions func,
  std::string name,
  std::string alias,
  const utils::basic_type type,
  const column_options options
) {
  return column{
    nullptr,
    std::move(name),
    std::move(alias),
    type,
    func,
    {},
    options,
    0
  };
}

column column::make_query_function(
  const query_functions func,
  const class table* tab,
  std::string name,
  std::string alias,
  const utils::basic_type type,
  const column_options options
) {
  return column{
    tab,
    std::move(name),
    std::move(alias),
    type,
    func,
    {},
    options,
    0
  };
}

column column::make_expression(
  const std::shared_ptr<abstract_column_expression>& expression,
  std::string alias
) {
  if (!expression) {
    throw std::invalid_argument("Expression column must not be null");
  }

  return column{
    nullptr,
    "",
    std::move(alias),
    utils::basic_type::Unknown,
    query_functions::None,
    expression,
    {},
    0
  };
}

column::column(const class table *tab,
               std::string name,
               std::string alias,
               const utils::basic_type type,
               const query_functions func,
               const std::shared_ptr<abstract_column_expression>& expression,
               const column_options& options,
               const size_t index)
: alias_(std::move(alias))
, options_(options) {
  plain_column plain{tab, std::move(name), index, type};

  if (expression) {
    value_ = expression;
  } else if (func != query_functions::None) {
    value_ = query_function{func, std::move(plain)};
  } else {
    value_ = std::move(plain);
  }
}

bool column::equals(const column &x) const {
  if (alias_ != x.alias_ || value_.index() != x.value_.index()) {
    return false;
  }

  if (const auto* expression = std::get_if<std::shared_ptr<abstract_column_expression>>(&value_)) {
    return *expression == std::get<std::shared_ptr<abstract_column_expression>>(x.value_);
  }

  const auto* plain_column = plain();
  const auto* other_plain_column = x.plain();
  if ((plain_column->table == nullptr) != (other_plain_column->table == nullptr) ||
      (plain_column->table != nullptr && !(*plain_column->table == *other_plain_column->table)) ||
      plain_column->name != other_plain_column->name ||
      plain_column->type != other_plain_column->type) {
    return false;
  }

  const auto* function = std::get_if<query_function>(&value_);
  return function == nullptr ||
         function->function == std::get<query_function>(x.value_).function;
}

column column::as(const std::string& alias) const {
  auto result = *this;
  result.alias_ = alias;
  return result;
}

std::string column::name() const {
  return canonical_name();
}

const std::string& column::column_name() const {
  static const std::string empty;
  const auto* plain_column = plain();
  return plain_column == nullptr ? empty : plain_column->name;
}

std::string column::canonical_name() const {
  const auto* plain_column = plain();
  return plain_column == nullptr ? std::string{} :
         build_canonical_name(plain_column->table, plain_column->name);
}

const std::string& column::alias() const {
  return alias_;
}

size_t column::index() const {
  const auto* plain_column = plain();
  return plain_column == nullptr ? 0 : plain_column->index;
}

const std::string& column::result_name() const {
  return alias_.empty() ? column_name() : alias_;
}

utils::basic_type column::type() const {
  const auto* plain_column = plain();
  return plain_column == nullptr ? utils::basic_type::Unknown : plain_column->type;
}

column_constraints column::constraints() const {
  return options_.constraints();
}

bool column::is_primary_key() const {
  return options_.constraints().has(column_constraint::PrimaryKey);
}

bool column::is_plain_column() const {
  return std::holds_alternative<plain_column>(value_);
}

bool column::is_function() const {
  return std::holds_alternative<query_function>(value_);
}

bool column::is_expression() const {
  const auto* expression = std::get_if<std::shared_ptr<abstract_column_expression>>(&value_);
  return expression != nullptr && static_cast<bool>(*expression);
}

bool column::is_nullable() const {
  return !options_.constraints().has(column_constraint::NotNull);
}

bool column::is_foreign_key() const {
  return options_.constraints().has(column_constraint::ForeignKey);
}

bool column::is_unique() const {
  return options_.constraints().has(column_constraint::Unique);
}

bool column::is_identity() const {
  return options_.constraints().has(column_constraint::Identity);
}

query_functions column::function() const {
  const auto* function = std::get_if<query_function>(&value_);
  return function == nullptr ? query_functions::None : function->function;
}

bool column::has_alias() const {
  return !alias_.empty();
}

const table* column::table() const {
  const auto* plain_column = plain();
  return plain_column == nullptr ? nullptr : plain_column->table;
}

void column::table(const class table* tab) {
  auto* plain_column = plain();
  if (plain_column == nullptr) {
    throw std::logic_error("Cannot attach a table to an expression column");
  }
  plain_column->table = tab;
}

column::operator std::string() const {
  return name();
}

std::shared_ptr<abstract_column_expression> column::expression() const {
  const auto* expression = std::get_if<std::shared_ptr<abstract_column_expression>>(&value_);
  return expression == nullptr ? nullptr : *expression;
}

std::string column::build_canonical_name(const class table* tab, const std::string& name) {
  return tab != nullptr ? tab->name() + "." + name : name;
}

const column::plain_column* column::plain() const {
  if (const auto* plain_value = std::get_if<plain_column>(&value_)) {
    return plain_value;
  }
  if (const auto* function = std::get_if<query_function>(&value_)) {
    return &function->column;
  }
  return nullptr;
}

column::plain_column* column::plain() {
  if (auto* plain_value = std::get_if<plain_column>(&value_)) {
    return plain_value;
  }
  if (auto* function = std::get_if<query_function>(&value_)) {
    return &function->column;
  }
  return nullptr;
}
}  // namespace matador::query
