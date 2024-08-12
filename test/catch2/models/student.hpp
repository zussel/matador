#ifndef QUERY_STUDENT_HPP
#define QUERY_STUDENT_HPP

#include "matador/utils/access.hpp"
#include "matador/utils/foreign_attributes.hpp"

#include "matador/object/object_ptr.hpp"
#include "matador/object/container.hpp"
#include "matador/object/has_many_to_many_item.hpp"

#include <utility>
#include <vector>

namespace matador::test {

struct course;

struct student
{
  unsigned long id{};
  std::string name;
  container<course> courses;

  student() = default;
  explicit student(unsigned long id, std::string name)
    : id(id)
    , name(std::move(name)) {}

  template < class Operator >
  void process(Operator &op)
  {
    namespace field = matador::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, 255);
    field::has_many_to_many(op, "student_courses", courses, "student_id", "course_id", utils::fetch_type::LAZY);
  }

};

struct course
{
  unsigned long id{};
  std::string title;
  container<student> students;

  course() = default;
  explicit course(unsigned long id, std::string title)
  : id(id)
  , title(std::move(title)) {}

  template < class Operator >
  void process(Operator &op)
  {
    namespace field = matador::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "title", title, 255);
    field::has_many_to_many(op, "student_courses", students, utils::fetch_type::EAGER);
  }
};

class student_course : public has_many_to_many_item<student, course>
{
public:
  student_course()
    : has_many_to_many_item("student_id", "course_id") {}
};

}
#endif //QUERY_STUDENT_HPP
