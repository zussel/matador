#ifndef MATADOR_ABSTRACT_COLUMN_CRITERIA_HPP
#define MATADOR_ABSTRACT_COLUMN_CRITERIA_HPP

#include "matador/query/criteria/abstract_criteria.hpp"

#include "matador/query/column.hpp"

namespace matador::query {

class abstract_column_criteria : public abstract_criteria {
public:
    abstract_column_criteria() = delete;
    explicit abstract_column_criteria(const column& col);

    [[nodiscard]] const column& col() const;

protected:
    column column_;
};

}
#endif //MATADOR_ABSTRACT_COLUMN_CRITERIA_HPP