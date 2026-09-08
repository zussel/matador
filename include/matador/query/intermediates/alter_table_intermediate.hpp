#ifndef MATADOR_QUERY_ALTER_TABLE_INTERMEDIATE_HPP
#define MATADOR_QUERY_ALTER_TABLE_INTERMEDIATE_HPP

#include "matador/query/intermediates/executable_query.hpp"
#include "matador/query/intermediates/intermediate.hpp"
#include "matador/query/column.hpp"
#include "matador/query/constraint.hpp"
#include "matador/query/table.hpp"

namespace matador::query {
class add_primary_key_constraint_intermediate final : public executable_query {
public:
};

class query_add_foreign_key_constraint_intermediate final : public intermediate {
public:
    using intermediate::intermediate;

    executable_query references(const table& tab, const std::vector<column>& columns = {});

private:
    table table_;
    std::vector<column> columns_;
};

class add_key_constraint_intermediate final : public intermediate {
public:
    using intermediate::intermediate;

    [[nodiscard]] add_primary_key_constraint_intermediate primary_key(const column& col) const;
    [[nodiscard]] add_primary_key_constraint_intermediate primary_keys(const std::vector<column>& columns) const;
    [[nodiscard]] query_add_foreign_key_constraint_intermediate foreign_key(const column& col) const;
    [[nodiscard]] query_add_foreign_key_constraint_intermediate foreign_keys(const std::vector<column>& columns) const;
};

class alter_table_intermediate final : public intermediate {
public:
    using intermediate::intermediate;

    executable_query add_constraint(const constraint& c);
    add_key_constraint_intermediate add_constraint(const std::string& name);
    executable_query drop_constraint(const constraint& c);
    executable_query drop_constraint(const std::string& name);
};
}

#endif //MATADOR_QUERY_ALTER_TABLE_INTERMEDIATE_HPP