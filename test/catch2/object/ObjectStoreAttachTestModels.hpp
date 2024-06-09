#ifndef MATADOR_OBJECTSTOREATTACHTESTMODELS_HPP
#define MATADOR_OBJECTSTOREATTACHTESTMODELS_HPP

#include "matador/utils/access.hpp"

namespace matador::test {

struct simple {
  unsigned long id{};
  std::string name;
  double value{};

  template<typename Operator>
  void process(Operator &op) {
    namespace field = matador::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, 255);
    field::attribute(op, "value", value);
  }
};

struct hammer
{
  unsigned long id{};
  std::string name;

  template<typename Operator>
  void process(Operator &op) {
    namespace field = matador::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, 255);
  }
};

struct toolbox
{
  unsigned long id{};
  std::string name;
  object_ptr<hammer> hammer_;

  template<typename Operator>
  void process(Operator &op) {
    namespace field = matador::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, 255);
    field::belongs_to(op, "hammer", hammer_);
  }
};

struct profile;

struct user
{
  unsigned long id{};
  std::string name;
  object_ptr<profile> profile_;

  template<typename Operator>
  void process(Operator &op) {
    namespace field = matador::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, 255);
    field::has_one(op, "id", profile_);
  }
};

struct profile
{
  unsigned long id{};
  std::string name;
  object_ptr<user> user_;

  template<typename Operator>
  void process(Operator &op) {
    namespace field = matador::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, 255);
    field::belongs_to(op, "user_id", user_);
  }
};

struct department;

struct employee
{
  unsigned long id{};
  std::string name;
  object_ptr<department> department_;

  template<typename Operator>
  void process(Operator &op) {
    namespace field = matador::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, 255);
    field::belongs_to(op, "department_id", department_);
  }
};

struct department
{
  unsigned long id{};
  std::string name;
  container<employee> employees;

  template<typename Operator>
  void process(Operator &op) {
    namespace field = matador::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, 255);
    field::has_many(op, "employees", employees, "id");
  }
};

struct course;

struct student
{
  unsigned long id{};
  std::string name;
  container<course> courses;

  template<typename Operator>
  void process(Operator &op) {
    namespace field = matador::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, 255);
    field::has_many_to_many(op, "student_course", courses, "student_id", "course_id");
  }
};

struct course
{
  unsigned long id{};
  std::string name;
  container<student> students;

  template<typename Operator>
  void process(Operator &op) {
    namespace field = matador::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, 255);
    field::has_many_to_many(op, "student_course", students);
  }
};

}

#endif //MATADOR_OBJECTSTOREATTACHTESTMODELS_HPP
