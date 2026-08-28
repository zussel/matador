#ifndef MATADOR_EXPRESSION_VISITOR_HPP
#define MATADOR_EXPRESSION_VISITOR_HPP

namespace matador::query {
class binary_column_expression;
class table_column_expression;
class value_expression;
class placeholder_expression;

class expression_visitor {
public:
  virtual ~expression_visitor() = default;

  virtual void visit(const binary_column_expression& node) = 0;
  virtual void visit(const table_column_expression& node) = 0;
  virtual void visit(const value_expression& node) = 0;
  virtual void visit(const placeholder_expression& node) = 0;
};
}
#endif //MATADOR_EXPRESSION_VISITOR_HPP