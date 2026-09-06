#ifndef CRITERIA_BETWEEN_CRITERIA_NODE_HPP
#define CRITERIA_BETWEEN_CRITERIA_NODE_HPP

#include "matador/query/criteria/abstract_column_criteria.hpp"
#include "matador/query/criteria/criteria_utils.hpp"
#include "matador/query/placeholder.hpp"

namespace matador::query {

class between_criteria final : public abstract_column_criteria {
public:
    between_criteria() = delete;
    between_criteria(const column& col, int64_t min, int64_t max);
    between_criteria(const column& col, placeholder min, placeholder max);

    void accept(criteria_visitor& visitor) const override;

    [[nodiscard]] const criteria_value &minimum() const;
    [[nodiscard]] const criteria_value &maximum() const;

private:
    criteria_value min_;
    criteria_value max_;
};
}
#endif //CRITERIA_BETWEEN_CRITERIA_NODE_HPP