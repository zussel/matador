#ifndef MATADOR_DTO_HPP
#define MATADOR_DTO_HPP

#include "matador/utils/base_class.hpp"
#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"

#include <set>
#include <unordered_set>

struct bounding_box
{
  long length = 0;
  long width = 0;
  long height = 0;

  bounding_box() = default;

  bounding_box(long l, long w, long h)
    : length(l), width(w), height(h)
  {}

  template < class S >
  void serialize(S &s)
  {
    s.on_attribute("length", length);
    s.on_attribute("width", width);
    s.on_attribute("height", height);
  }
};

struct dto
{
  std::string id;
  std::string name;
  matador::date birthday;
  matador::time created;
  bool flag = false;
  long height = 0;
  std::vector<double> doubles;
  std::list<bool> bits;
  std::set<std::string> names;
  std::unordered_set<int> values;
  bounding_box dimension;
  std::vector<bounding_box> dimensions;

  template < class S >
  void serialize(S &s)
  {
    s.on_primary_key("id", id, 255);
    s.on_attribute("name", name);
    s.on_attribute("birthday", birthday);
    s.on_attribute("created", created);
    s.on_attribute("flag", flag);
    s.on_attribute("height", height);
    s.on_attribute("doubles", doubles);
    s.on_attribute("bits", bits);
    s.on_attribute("names", names);
    s.on_attribute("values", values);
    s.on_attribute("dimension", dimension);
    s.on_attribute("dimensions", dimensions);
  }
};

struct base
{
  long id;

  template < class S >
  void serialize(S &s)
  {
    s.on_attribute("id", id);
  }
};

struct derived : public base
{
  std::string name;

  template < class S >
  void serialize(S &s)
  {
    s.serialize(*matador::base_class<base>(this));
    s.on_attribute("name", name);
  }
};

#endif //MATADOR_DTO_HPP
