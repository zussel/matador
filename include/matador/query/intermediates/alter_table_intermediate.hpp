#ifndef MATADOR_QUERY_ALTER_TABLE_INTERMEDIATE_HPP
#define MATADOR_QUERY_ALTER_TABLE_INTERMEDIATE_HPP

#include "matador/query/model/column.hpp"
#include "matador/query/model/constraint.hpp"
#include "matador/query/intermediates/executable_query.hpp"
#include "matador/query/intermediates/intermediate.hpp"

namespace matador::query {
class add_primary_key_constraint_intermediate final : public executable_query {
public:
};

class query_add_foreign_key_constraint_intermediate final : public intermediate {
public:
    using intermediate::intermediate;

    [[nodiscard]] executable_query references(const std::string& table_name, const std::vector<column>& columns = {}) const;
    [[nodiscard]] executable_query references(const table& tab, const std::vector<column>& columns = {}) const;

private:
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

    executable_query add_constraint(const constraint& c) const;
    add_key_constraint_intermediate add_constraint(const std::string& name) const;
    executable_query drop_constraint(const constraint& c) const;
    executable_query drop_constraint(const std::string& name) const;
};
}

#endif //MATADOR_QUERY_ALTER_TABLE_INTERMEDIATE_HPP