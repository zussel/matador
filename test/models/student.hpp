#ifndef QUERY_STUDENT_HPP
#define QUERY_STUDENT_HPP

#include "matador/query/relations/collection.hpp"
#include "matador/query/relations/object_ptr.hpp"
#include "matador/query/mapping/access.hpp"
#include "matador/query/model/foreign_key_options.hpp"

#include <utility>

namespace matador::test {
struct course;

struct student {
  unsigned int id{};
  std::string name;
  query::collection<query::object_ptr<course> > courses;

  student() = default;

  explicit student(const unsigned int id, std::string name)
  : id(id)
  , name(std::move(name)) {
  }

  template<class Operator>
  void process(Operator &op) {
    namespace field = query::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, UniqueVarChar255);
    field::has_many_to_many(op, "student_courses", courses, "student_id", "course_id", CascadeAllFetchLazy);
  }
};

struct course {
  unsigned int id{};
  std::string title;
  query::collection<query::object_ptr<student> > students;

  course() = default;

  explicit course(const unsigned int id, std::string title)
  : id(id)
  , title(std::move(title)) {
  }

  template<class Operator>
  void process(Operator &op) {
    namespace field = query::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "title", title, UniqueVarChar255);
    field::has_many_to_many(op, "student_courses", students, CascadeAllFetchEager);
  }
};
}
#endif //QUERY_STUDENT_HPP
