#ifndef MATADOR_EXPRESSION_EVALUATOR_HPP
#define MATADOR_EXPRESSION_EVALUATOR_HPP

#include "matador/query/expression/expression_visitor.hpp"

#include <string>

namespace matador::query {
class dialect;
struct query_context;
class expression_evaluator final : public expression_visitor {
public:
    expression_evaluator(const dialect &d, query_context &query);
    void visit(const binary_column_expression& node) override;
    void visit(const table_column_expression& node) override;
    void visit(const value_expression& node) override;
    void visit(const placeholder_expression& node) override;

    [[nodiscard]] const std::string& result() const;

private:
    const dialect &dialect_;
    query_context &query_;
    std::string expression_;
};
}
#endif //MATADOR_EXPRESSION_EVALUATOR_HPP