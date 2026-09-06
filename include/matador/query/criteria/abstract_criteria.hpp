#ifndef CRITERIA_CRITERIA_NODE_HPP
#define CRITERIA_CRITERIA_NODE_HPP

#include <memory>

namespace matador::query {

class criteria_visitor;

class abstract_criteria {
public:
    virtual ~abstract_criteria() = default;

    virtual void accept(criteria_visitor& visitor) const = 0;
};

using criteria_ptr = std::unique_ptr<abstract_criteria>;

}
#endif //CRITERIA_CRITERIA_NODE_HPP