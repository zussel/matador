#ifndef MATADOR_ABSTRACT_COLUMN_EXPRESSION_HPP
#define MATADOR_ABSTRACT_COLUMN_EXPRESSION_HPP

#include <memory>

namespace matador::query {
class expression_visitor;
class query_column;

class abstract_column_expression {
public:
    virtual ~abstract_column_expression() = default;

    virtual void accept(expression_visitor& visitor) const = 0;
};

using column_expression_ptr = std::unique_ptr<abstract_column_expression>;
}
#endif //MATADOR_ABSTRACT_COLUMN_EXPRESSION_HPP