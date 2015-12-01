//
// Created by sascha on 01.12.15.
//

#ifndef OOS_STUDENT_HPP
#define OOS_STUDENT_HPP

#include "person.hpp"

#include <string>

class student : public person
{
public:

  student() : person() { }
  student(const std::string &name, int age, const std::string &em)
    : person(name, age), email_(em)
  { }

  const std::string email() const {
    return email_;
  }

  void email(const std::string &em) {
    student::email_ = em;
  }

private:
  std::string email_;
};

#endif //OOS_STUDENT_HPP
