#ifndef CRITERIA_COLLECTION_CRITERIA_NODE_HPP
#define CRITERIA_COLLECTION_CRITERIA_NODE_HPP

#include "matador/query/criteria/abstract_column_criteria.hpp"
#include "matador/query/criteria/criteria_utils.hpp"
#include "matador/query/query_context.hpp"
#include "matador/query/table.hpp"

namespace matador::query {
class column;

enum class collection_operator {
  In,
  Out
};

/**
 * @brief Criteria class representing an IN condition
 *
 * This class represents a query IN condition and evaluates to
 * this condition based on the current database d
 *
 * @code
 * WHERE age IN (29,34,56)
 * @endcode
 */
class collection_criteria final : public abstract_column_criteria {
public:
    collection_criteria() = delete;
    /**
     * @brief Creates an IN condition
     *
     * Creates an IN or OUT criteria for the given sql::column and
     * the given list of arguments.
     *
     * @param col Column for the IN condition
     * @param operand_ Operator to use
     * @param values List of values
     */
    collection_criteria(const column& col, collection_operator operand_, std::vector<criteria_value> values);
    collection_criteria(const column& col, collection_operator operand_, std::initializer_list<criteria_value> values);

    void accept(criteria_visitor& visitor) const override;

    [[nodiscard]] collection_operator operand() const;
    [[nodiscard]] const std::vector<criteria_value>& values() const;

private:
    collection_operator operand_;
    std::vector<criteria_value> values_;
};

class collection_query_criteria final : public abstract_column_criteria {
public:
    collection_query_criteria() = delete;
    collection_query_criteria(const column& col, collection_operator operand_, fetchable_query  q);

    void accept(criteria_visitor& visitor) const override;

    [[nodiscard]] collection_operator operand() const;
    [[nodiscard]] const fetchable_query& query() const;

private:
    collection_operator operand_;
    fetchable_query query_context_;
};
}
#endif //CRITERIA_COLLECTION_CRITERIA_NODE_HPP