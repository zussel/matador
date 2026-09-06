#ifndef CRITERIA_LIKE_CRITERIA_NODE_HPP
#define CRITERIA_LIKE_CRITERIA_NODE_HPP

#include "matador/query/criteria/abstract_column_criteria.hpp"

namespace matador::query {
class column;

class like_criteria final : public abstract_column_criteria {
public:
  like_criteria() = delete;
  like_criteria(const column& col, std::string pattern);

  void accept(criteria_visitor &visitor) const override;

  [[nodiscard]] const std::string& pattern() const;

private:
  std::string pattern_;
};
}
#endif //CRITERIA_LIKE_CRITERIA_NODE_HPP