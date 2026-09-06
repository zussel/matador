#ifndef CRITERIA_BINARY_CRITERIA_NODE_HPP
#define CRITERIA_BINARY_CRITERIA_NODE_HPP

#include "matador/query/criteria/abstract_column_criteria.hpp"
#include "matador/query/criteria/criteria_utils.hpp"
#include "matador/query/column.hpp"

namespace matador::query {
enum class binary_operator {
    Equals,
    NotEquals,
    GreaterThan,
    GreaterThanOrEqual,
    LessThan,
    LessThanOrEqual,
};

class binary_criteria final : public abstract_column_criteria {
public:
    binary_criteria() = delete;
    binary_criteria(const column& col, binary_operator operand, criteria_value value);

    void accept( criteria_visitor& visitor ) const override;

    [[nodiscard]] binary_operator operand() const;
    [[nodiscard]] const criteria_value& value() const;

private:
    binary_operator operator_{};
    criteria_value value_;
};

class binary_column_criteria final : public abstract_criteria {
public:
    binary_column_criteria() = delete;
    binary_column_criteria(column  left_column, binary_operator operand, column  right_column);

    void accept(criteria_visitor& visitor) const override;

    [[nodiscard]] const column& left_column() const;
    [[nodiscard]] binary_operator operand() const;
    [[nodiscard]] const column& right_column() const;

private:
    column left_column_;
    binary_operator operator_{};
    column right_column_;
};
}
#endif //CRITERIA_BINARY_CRITERIA_NODE_HPP