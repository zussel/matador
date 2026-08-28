#include "matador/query/expression/column_expression.hpp"

namespace matador::query {
column_expression::column_expression(column_expression_ptr expr) noexcept
: expression_(std::move(expr)) {}

column_expression::column_expression(placeholder /*p*/)
: expression_(std::make_unique<placeholder_expression>()){
}

bool column_expression::empty() const noexcept {
  return !expression_;
}

const abstract_column_expression* column_expression::get() const noexcept {
   return expression_.get();
}

const column_expression_ptr& column_expression::ptr() const noexcept {
  return expression_;
}

column_expression_ptr column_expression::release() noexcept {
  return std::move(expression_);
}

column_expression::operator std::unique_ptr<abstract_column_expression>() && noexcept {
  return std::move(expression_);
}

column column_expression::as(const std::string& alias) && {
    const column col{std::shared_ptr<abstract_column_expression>(expression_.release())};
    return col.as(alias);
}
}
