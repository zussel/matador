#ifndef CRITERIA_NOT_CRITERIA_NODE_HPP
#define CRITERIA_NOT_CRITERIA_NODE_HPP

#include "matador/query/criteria/abstract_criteria.hpp"

namespace matador::query {
class not_criteria final : public abstract_criteria {
public:
  explicit not_criteria(criteria_ptr clause);

  void accept(criteria_visitor& visitor) const override;

  [[nodiscard]] const criteria_ptr& clause() const;

private:
  criteria_ptr criteria_;
};
}

#endif //CRITERIA_NOT_CRITERIA_NODE_HPP