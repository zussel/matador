//
// Created by sascha on 11/25/15.
//

#ifndef PERSON_HPP
#define PERSON_HPP

#include "tools/identifier.hpp"
#include "tools/access.hpp"

#include <string>

class person
{
public:
  typedef oos::identifier<unsigned long> ul_id;
  typedef ul_id::type id_type;

private:
  ul_id id_;
  std::string name_;
  int age_;

  static id_type id_counter;

  friend class oos::access;

  template < class serializer > void serialize(serializer &s) const;
  template < class serializer > void deserialize(serializer &s);

public:
  person() : id_(++id_counter), age_(0) {}
  person(const std::string &name, int age)
    : id_(++id_counter), name_(name), age_(age)
  {}
  virtual ~person() {}

  id_type id() const { return id_.value(); }

  std::string name() const { return name_; }
  void name(const std::string &name) { name_ = name; }

  int age() const { return age_; }
  void age(int age) { age_ = age; }

};

person::id_type person::id_counter = 0;

template < class serializer >
void person::serialize(serializer &s) const
{
  s.serialize("id", id_);
  s.serialize("name", name_);
  s.serialize("age", age_);
}

template < class serializer >
void person::deserialize(serializer &s)
{
  s.deserialize("id", id_);
  s.deserialize("name", name_);
  s.deserialize("age", age_);
}

#endif /* PERSON_HPP */
