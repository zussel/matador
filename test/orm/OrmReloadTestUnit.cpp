#include "OrmReloadTestUnit.hpp"

#include "../Item.hpp"
#include "../has_many_list.hpp"

#include "matador/orm/persistence.hpp"
#include "matador/orm/session.hpp"

#include "matador/object/object_view.hpp"

using namespace hasmanylist;

OrmReloadTestUnit::OrmReloadTestUnit(const std::string &prefix, const std::string &dns)
  : unit_test(prefix + "_orm_reload", prefix + " orm reload test unit")
  , dns_(dns)
{
  add_test("load", std::bind(&OrmReloadTestUnit::test_load, this), "test load from table");
  add_test("load_has_one", std::bind(&OrmReloadTestUnit::test_load_has_one, this), "test load has one relation from table");
  add_test("load_has_many", std::bind(&OrmReloadTestUnit::test_load_has_many, this), "test load has many from table");
  add_test("load_has_many_to_many", std::bind(&OrmReloadTestUnit::test_load_has_many_to_many, this), "test load has many to many from table");
//  add_test("load_has_many_to_many_remove", std::bind(&OrmReloadTestUnit::test_load_has_many_to_many_remove, this), "test load has many to many from table with remove");
  add_test("load_has_many_int", std::bind(&OrmReloadTestUnit::test_load_has_many_int, this), "test load has many int from table");
}

void OrmReloadTestUnit::test_load()
{
  matador::persistence p(dns_);

  p.attach<person>("person");

  p.create();

  std::vector<std::string> names({"hans", "otto", "georg", "hilde", "ute", "manfred"});

  {
    // insert some persons
    matador::session s(p);

    for (const std::string &name : names) {
      auto pptr = s.insert(new person(name, matador::date(18, 5, 1980), 180));

      UNIT_EXPECT_GREATER(pptr->id(), 0UL, "is must be greater zero");
    }
  }

  p.clear();

  {
    // load persons from database
    matador::session s(p);

    s.load();

    typedef matador::object_view<person> t_person_view;
    t_person_view persons(s.store());

    UNIT_ASSERT_TRUE(!persons.empty(), "person view must not be empty");
    UNIT_ASSERT_EQUAL(persons.size(), 6UL, "thier must be 6 persons");

    t_person_view::iterator first = persons.begin();
    t_person_view::iterator last = persons.end();

    while (first != last) {
      auto pptr = *first++;
      names.erase(std::remove_if(std::begin(names), std::end(names), [pptr](const std::string &name) {
        return name == pptr->name();
      }), names.end());
    }
    UNIT_ASSERT_TRUE(names.empty(), "names must be empty");
  }

  p.drop();
}

void OrmReloadTestUnit::test_load_has_one()
{
  matador::persistence p(dns_);

  p.attach<master>("master");
  p.attach<child>("child");

  p.create();

  {
    // insert some persons
    matador::session s(p);

    auto c = s.insert(new child("child 1"));

    s.insert(new master("master 1", c));
  }

  p.clear();

  {
    // load persons from database
    matador::session s(p);

    s.load();

    typedef matador::object_view<master> t_master_view;
    t_master_view masters(s.store());

    UNIT_ASSERT_TRUE(!masters.empty(), "master view must not be empty");
    UNIT_ASSERT_EQUAL(masters.size(), 1UL, "their must be 1 master");

    auto mptr = masters.front();
    UNIT_ASSERT_NOT_NULL(mptr->children.get(), "child must be valid");

    typedef matador::object_view<child> t_child_view;
    t_child_view childview(s.store());

    UNIT_ASSERT_TRUE(!childview.empty(), "master view must not be empty");
    UNIT_ASSERT_EQUAL(childview.size(), 1UL, "their must be 1 master");

    auto chptr = childview.front();

    UNIT_ASSERT_TRUE(chptr == mptr->children, "objects must be the same");
  }

  p.drop();
}

void OrmReloadTestUnit::test_load_has_many()
{
  matador::persistence p(dns_);

  p.attach<child>("child");
  p.attach<children_list>("children_list");

  p.create();

  {
    matador::session s(p);

    auto children = s.insert(new children_list("children list 1"));

    UNIT_ASSERT_GREATER(children->id, 0UL, "invalid children list");
    UNIT_ASSERT_TRUE(children->children.empty(), "children list must be empty");

    auto kid1 = s.insert(new child("kid 1"));
    auto kid2 = s.insert(new child("kid 2"));

    UNIT_ASSERT_GREATER(kid1->id, 0UL, "invalid child");
    UNIT_ASSERT_GREATER(kid2->id, 0UL, "invalid child");

    s.push_back(children->children, kid1);
    s.push_back(children->children, kid2);

    UNIT_ASSERT_FALSE(children->children.empty(), "children list couldn't be empty");
    UNIT_ASSERT_EQUAL(children->children.size(), 2UL, "invalid children list size");
  }

  p.clear();

  {
    matador::session s(p);

    s.load();

    typedef matador::object_view<children_list> t_children_list_view;
    t_children_list_view children_lists(s.store());

    UNIT_ASSERT_TRUE(!children_lists.empty(), "children lists view must not be empty");
    UNIT_ASSERT_EQUAL(children_lists.size(), 1UL, "their must be 1 children list");

    auto clptr = children_lists.front();

    UNIT_ASSERT_FALSE(clptr->children.empty(), "children list couldn't be empty");
    UNIT_ASSERT_EQUAL(clptr->children.size(), 2UL, "invalid children list size");

    std::vector<std::string> result_names({ "kid 1", "kid 2"});
    for (auto kid : clptr->children) {
      auto it = std::find(result_names.begin(), result_names.end(), kid->name);
      UNIT_EXPECT_FALSE(it == result_names.end(), "kid must be found");
    }
  }

  p.drop();
}

void OrmReloadTestUnit::test_load_has_many_to_many()
{
  matador::persistence p(dns_);

  p.attach<person>("person");
  p.attach<student, person>("student");
  p.attach<course>("course");

  p.create();

  {
    matador::session s(p);

    auto jane = s.insert(new student("jane"));
    auto tom = s.insert(new student("tom"));
    auto art = s.insert(new course("art"));

    UNIT_ASSERT_TRUE(jane->courses.empty(), "vector must be empty");
    UNIT_ASSERT_EQUAL(jane->courses.size(), 0UL, "vector size must be zero");
    UNIT_ASSERT_TRUE(tom->courses.empty(), "vector must be empty");
    UNIT_ASSERT_EQUAL(tom->courses.size(), 0UL, "vector size must be zero");
    UNIT_ASSERT_TRUE(art->students.empty(), "vector must be empty");
    UNIT_ASSERT_EQUAL(art->students.size(), 0UL, "vector size must be zero");

    s.push_back(jane->courses, art); // jane (value) must be push_back to course art (owner) students!!

    UNIT_ASSERT_FALSE(jane->courses.empty(), "vector must not be empty");
    UNIT_ASSERT_EQUAL(jane->courses.size(), 1UL, "vector size must be one");
    UNIT_ASSERT_EQUAL(jane->courses.front(), art, "objects must be same");
    UNIT_ASSERT_FALSE(art->students.empty(), "vector must not be empty");
    UNIT_ASSERT_EQUAL(art->students.size(), 1UL, "vector size must be zero");
    UNIT_ASSERT_EQUAL(art->students.front(), jane, "objects must be same");

    s.push_back(art->students, tom);

    UNIT_ASSERT_FALSE(tom->courses.empty(), "vector must not be empty");
    UNIT_ASSERT_EQUAL(tom->courses.size(), 1UL, "vector size must be one");
    UNIT_ASSERT_EQUAL(tom->courses.front(), art, "objects must be same");
    UNIT_ASSERT_FALSE(art->students.empty(), "vector must not be empty");
    UNIT_ASSERT_EQUAL(art->students.size(), 2UL, "vector size must be zero");
    UNIT_ASSERT_EQUAL(art->students.back(), tom, "objects must be same");
  }

  p.clear();

  {
    matador::session s(p);

    s.load();

    typedef matador::object_view<student> t_student_view;
    t_student_view student_view(s.store());

    UNIT_ASSERT_TRUE(!student_view.empty(), "student view must not be empty");
    UNIT_ASSERT_EQUAL(student_view.size(), 2UL, "their must be 2 student in list");

    typedef matador::object_view<course> t_course_view;
    t_course_view course_view(s.store());

    UNIT_ASSERT_TRUE(!course_view.empty(), "course view must not be empty");
    UNIT_ASSERT_EQUAL(course_view.size(), 1UL, "their must be 1 course in list");

    for (const auto &stdnt : student_view) {
      UNIT_ASSERT_FALSE(stdnt->courses.empty(), "vector must not be empty");
      UNIT_ASSERT_EQUAL(stdnt->courses.size(), 1UL, "vector size must be one");
    }

    auto art = course_view.front();
    UNIT_ASSERT_FALSE(art->students.empty(), "vector must not be empty");
    UNIT_ASSERT_EQUAL(art->students.size(), 2UL, "vector size must be two");

    auto stud = student_view.front();

    s.remove(art->students, stud);

    UNIT_ASSERT_FALSE(art->students.empty(), "vector must not be empty");
    UNIT_ASSERT_EQUAL(art->students.size(), 1UL, "vector size must be two");
  }

  p.drop();
}

void OrmReloadTestUnit::test_load_has_many_to_many_remove()
{
  matador::persistence p(dns_);

  p.attach<person>("person");
  p.attach<student, person>("student");
  p.attach<course>("course");

  p.create();

  {
    matador::session s(p);

    auto jane = s.insert(new student("jane"));
    auto tom = s.insert(new student("tom"));
    auto art = s.insert(new course("art"));

    UNIT_ASSERT_TRUE(jane->courses.empty(), "vector must be empty");
    UNIT_ASSERT_EQUAL(jane->courses.size(), 0UL, "vector size must be zero");
    UNIT_ASSERT_TRUE(tom->courses.empty(), "vector must be empty");
    UNIT_ASSERT_EQUAL(tom->courses.size(), 0UL, "vector size must be zero");
    UNIT_ASSERT_TRUE(art->students.empty(), "vector must be empty");
    UNIT_ASSERT_EQUAL(art->students.size(), 0UL, "vector size must be zero");

    s.push_back(jane->courses, art); // jane (value) must be push_back to course art (owner) students!!

    UNIT_ASSERT_FALSE(jane->courses.empty(), "vector must not be empty");
    UNIT_ASSERT_EQUAL(jane->courses.size(), 1UL, "vector size must be one");
    UNIT_ASSERT_EQUAL(jane->courses.front(), art, "objects must be same");
    UNIT_ASSERT_FALSE(art->students.empty(), "vector must not be empty");
    UNIT_ASSERT_EQUAL(art->students.size(), 1UL, "vector size must be zero");
    UNIT_ASSERT_EQUAL(art->students.front(), jane, "objects must be same");

    s.push_back(art->students, tom);

    UNIT_ASSERT_FALSE(tom->courses.empty(), "vector must not be empty");
    UNIT_ASSERT_EQUAL(tom->courses.size(), 1UL, "vector size must be one");
    UNIT_ASSERT_EQUAL(tom->courses.front(), art, "objects must be same");
    UNIT_ASSERT_FALSE(art->students.empty(), "vector must not be empty");
    UNIT_ASSERT_EQUAL(art->students.size(), 2UL, "vector size must be zero");
    UNIT_ASSERT_EQUAL(art->students.back(), tom, "objects must be same");

    s.remove(art->students, tom);

    UNIT_ASSERT_TRUE(tom->courses.empty(), "vector must not be empty");
    UNIT_ASSERT_EQUAL(tom->courses.size(), 0UL, "vector size must be one");
    UNIT_ASSERT_FALSE(art->students.empty(), "vector must not be empty");
    UNIT_ASSERT_EQUAL(art->students.size(), 1UL, "vector size must be zero");
    UNIT_ASSERT_EQUAL(art->students.back(), jane, "objects must be same");

    s.push_back(art->students, tom);

    UNIT_ASSERT_FALSE(tom->courses.empty(), "vector must not be empty");
    UNIT_ASSERT_EQUAL(tom->courses.size(), 1UL, "vector size must be one");
    UNIT_ASSERT_EQUAL(tom->courses.front(), art, "objects must be same");
    UNIT_ASSERT_FALSE(art->students.empty(), "vector must not be empty");
    UNIT_ASSERT_EQUAL(art->students.size(), 2UL, "vector size must be zero");
    UNIT_ASSERT_EQUAL(art->students.back(), tom, "objects must be same");
  }

  p.clear();

  {
    matador::session s(p);

    s.load();

    typedef matador::object_view<student> t_student_view;
    t_student_view student_view(s.store());

    UNIT_ASSERT_TRUE(!student_view.empty(), "student view must not be empty");
    UNIT_ASSERT_EQUAL(student_view.size(), 2UL, "their must be 2 student in list");

    typedef matador::object_view<course> t_course_view;
    t_course_view course_view(s.store());

    UNIT_ASSERT_TRUE(!course_view.empty(), "course view must not be empty");
    UNIT_ASSERT_EQUAL(course_view.size(), 1UL, "their must be 1 course in list");

    for (const auto &stdnt : student_view) {
      UNIT_ASSERT_FALSE(stdnt->courses.empty(), "vector must not be empty");
      UNIT_ASSERT_EQUAL(stdnt->courses.size(), 1UL, "vector size must be one");
    }

    auto art = course_view.front();
    UNIT_ASSERT_FALSE(art->students.empty(), "vector must not be empty");
    UNIT_ASSERT_EQUAL(art->students.size(), 2UL, "vector size must be two");

    auto stud = student_view.front();

    s.remove(art->students, stud);

    UNIT_ASSERT_FALSE(art->students.empty(), "vector must not be empty");
    UNIT_ASSERT_EQUAL(art->students.size(), 1UL, "vector size must be two");
  }

  p.clear();

  {
    matador::session s(p);

    s.load();

    typedef matador::object_view<student> t_student_view;
    t_student_view student_view(s.store());

    UNIT_ASSERT_TRUE(!student_view.empty(), "student view must not be empty");
    UNIT_ASSERT_EQUAL(student_view.size(), 2UL, "their must be 2 student in list");

    typedef matador::object_view<course> t_course_view;
    t_course_view course_view(s.store());

    UNIT_ASSERT_TRUE(!course_view.empty(), "course view must not be empty");
    UNIT_ASSERT_EQUAL(course_view.size(), 1UL, "their must be 1 course in list");

    auto art = course_view.front();
    UNIT_ASSERT_FALSE(art->students.empty(), "vector must not be empty");
    UNIT_ASSERT_EQUAL(art->students.size(), 1UL, "vector size must be one");
  }

  p.drop();
}

void OrmReloadTestUnit::test_load_has_many_int()
{
  matador::persistence p(dns_);

  p.attach<many_ints>("many_ints");

  p.create();

  {
    matador::session s(p);

    auto intlist = s.insert(new many_ints);

    UNIT_ASSERT_GREATER(intlist->id, 0UL, "invalid intlist list");
    UNIT_ASSERT_TRUE(intlist->elements.empty(), "intlist list must be empty");

    s.push_back(intlist->elements, 4);

    UNIT_ASSERT_EQUAL(intlist->elements.front(), 4, "first int must be 4");
    UNIT_ASSERT_EQUAL(intlist->elements.back(), 4, "last int must be 4");

    s.push_front(intlist->elements, 7);

    UNIT_ASSERT_EQUAL(intlist->elements.front(), 7, "first int must be 7");

    UNIT_ASSERT_FALSE(intlist->elements.empty(), "intlist list couldn't be empty");
    UNIT_ASSERT_EQUAL(intlist->elements.size(), 2UL, "invalid intlist list size");
  }

  p.clear();

  {
    matador::session s(p);

    s.load();

    typedef matador::object_view<many_ints> t_many_ints_view;
    t_many_ints_view ints_view(s.store());

    UNIT_ASSERT_TRUE(!ints_view.empty(), "many ints view must not be empty");
    UNIT_ASSERT_EQUAL(ints_view.size(), 1UL, "their must be 1 int in many ints list");

    auto intlist = ints_view.front();

    UNIT_ASSERT_FALSE(intlist->elements.empty(), "intlist list couldn't be empty");
    UNIT_ASSERT_EQUAL(intlist->elements.size(), 2UL, "invalid intlist list size");

    std::vector<int> result_ints({ 4, 7 });
    for (auto i : intlist->elements) {
      auto it = std::find(result_ints.begin(), result_ints.end(), i);
      UNIT_EXPECT_FALSE(it == result_ints.end(), "int must be found");
    }

  }

  p.drop();
}
