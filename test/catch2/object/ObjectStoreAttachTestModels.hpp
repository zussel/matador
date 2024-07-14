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

struct hammer_box
{
  unsigned long id{};
  std::string name;
  container<hammer> hammers_;

  template<typename Operator>
  void process(Operator &op) {
    namespace field = matador::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, 255);
    field::has_many(op, "hammers", hammers_);
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

struct post
{
  unsigned long id{};
  std::string name;
  matador::container<std::string> tags{255};

  template<typename Operator>
  void process(Operator &op) {
    namespace field = matador::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, 255);
    field::has_many(op, "tags", tags);
  }
};

struct department;

struct employee
{
  unsigned long id{};
  std::string name;
  matador::object_ptr<department> dep{};

  employee() = default;
  explicit employee(std::string name) : name(std::move(name)) {}

  template < class Operator >
  void process(Operator &op)
  {
    namespace field = matador::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name);
    field::belongs_to(op, "department"    , dep, matador::cascade_type::NONE);
    //                     ^ name of foreign table
    //                                      ^ object
    //                                           ^ foreign attributes
  }
};

struct department
{
  unsigned long id{};
  std::string name;
  matador::container<employee> employees{};

  department() = default;
  explicit department(std::string n)
  : name(std::move(n))
  {}

  ~department() = default;

  template < class Operator >
  void process(Operator &op)
  {
    matador::access::primary_key(op, "id", id);
    matador::access::attribute(op, "name", name, 255);
    matador::access::has_many(op, "employees", employees, "department", matador::cascade_type::NONE);
    //                             ^ id
    //                                         ^ container
    //                                                    ^ name of foreign join column
    //                                                                 ^ foreign attributes
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
