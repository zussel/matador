#include <iostream>

#include "matador/query/collection.hpp"
#include "matador/query/object_ptr.hpp"
#include "matador/query/schema.hpp"

using namespace matador;

struct department;
struct employee;

struct department {
  unsigned int id{};
  std::string name;
  collection<object_ptr<employee>> employees;

  template<typename Operator>
  void process(Operator &op) {
    namespace field = query::field;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, VarChar63);
    field::has_many(op, "employees", employees, "department_id", CascadeAllFetchLazy);
  }
};

struct employee {
  unsigned int id{};
  std::string name;
  object_ptr<department> department_;

  template<typename Operator>
  void process(Operator &op) {
    namespace field = query::field;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, matador::VarChar63);
    field::belongs_to(op, "department_id", department_, CascadeAllFetchLazy);
  }
};

int main() {
  schema s("main");

  auto result = s.attach<department>("departments").and_then([&s] {
    return s.attach<employee>("employees");
  });

  for (const auto& it : s) {
    std::cout << it.name() << "\n";
  }
}