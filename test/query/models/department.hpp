#ifndef DEPARTMENT_EMPLOYEE_HPP
#define DEPARTMENT_EMPLOYEE_HPP

#include "matador/query/object_ptr.hpp"
#include "matador/query/collection.hpp"

#include <string>
#include <vector>

namespace matador::test {

struct employee;

struct department {
    department() = default;
    department(const unsigned int id, std::string name)
    : id(id), name(std::move(name)) {}
    unsigned int id{};
    std::string name;
    query::collection<query::object_ptr<employee>> employees{};
    // object::object_ptr<employee> manager;

    template<typename Operator>
    void process(Operator &op) {
        query::field::primary_key(op, "id", id);
        query::field::attribute(op, "name", name, UniqueVarChar63);
        query::field::has_many(op, "employees", employees, "dep_id", query::CascadeAllFetchEager);
        // field::belongs_to(op, "manager_id", manager, utils::fetch_type::EAGER);
    }
};

struct employee {
    employee() = default;
    employee(const unsigned int id, std::string first, std::string last, query::object_ptr<department> dep)
    : id(id), first_name(std::move(first)), last_name(std::move(last)), dep(std::move(dep)) {}
    unsigned int id{};
    std::string first_name;
    std::string last_name;
    query::object_ptr<department> dep;

    template<typename Operator>
    void process(Operator &op) {
        query::field::primary_key(op, "id", id);
        query::field::attribute(op, "first_name", first_name, VarChar63);
        query::field::attribute(op, "last_name", last_name, VarChar63);
        query::field::belongs_to(op, "dep_id", dep, query::CascadeAllFetchLazy);
    }
};

}

#endif //DEPARTMENT_EMPLOYEE_HPP
