#ifndef CRITERIA_CRITERIA_NODE_VISITOR_HPP
#define CRITERIA_CRITERIA_NODE_VISITOR_HPP

namespace matador::query {
class between_criteria;
class binary_criteria;
class binary_column_criteria;
class check_null_criteria;
class like_criteria;
class logical_criteria;
class not_criteria;
class collection_criteria;
class collection_query_criteria;

class criteria_visitor {
public:
    virtual ~criteria_visitor() = default;

    virtual void visit(const between_criteria &node) = 0;
    virtual void visit(const binary_criteria &node) = 0;
    virtual void visit(const binary_column_criteria &node) = 0;
    virtual void visit(const check_null_criteria &node) = 0;
    virtual void visit(const collection_criteria &node) = 0;
    virtual void visit(const collection_query_criteria &node) = 0;
    virtual void visit(const like_criteria &node) = 0;
    virtual void visit(const logical_criteria &node) = 0;
    virtual void visit(const not_criteria &node) = 0;
};
}
#endif //CRITERIA_CRITERIA_NODE_VISITOR_HPP