#ifndef CRITERIA_LOGICAL_CRITERIA_NODE_HPP
#define CRITERIA_LOGICAL_CRITERIA_NODE_HPP

#include "matador/query/criteria/abstract_criteria.hpp"

namespace matador::query {
enum class logical_operator {
    AND,
    OR,
};

class logical_criteria final : public abstract_criteria {
public:
    logical_criteria() = delete;
    logical_criteria(criteria_ptr left, logical_operator op, criteria_ptr right);

    void accept(criteria_visitor& visitor) const override;

    [[nodiscard]] const criteria_ptr& left_clause() const;
    [[nodiscard]] logical_operator operand() const;
    [[nodiscard]] const criteria_ptr& right_clause() const;

private:
    criteria_ptr left_criteria_;
    logical_operator operand_;
    criteria_ptr right_criteria_;
};
}
#endif //CRITERIA_LOGICAL_CRITERIA_NODE_HPP