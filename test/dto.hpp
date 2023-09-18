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

  template < class Operator >
  void process(Operator &op)
  {
    matador::access::attribute(op, "length", length);
    matador::access::attribute(op, "width", width);
    matador::access::attribute(op, "height", height);
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

  template < class Operator >
  void process(Operator &op)
  {
    matador::access::primary_key(op, "id", id, 255);
    matador::access::attribute(op, "name", name);
    matador::access::attribute(op, "birthday", birthday);
    matador::access::attribute(op, "created", created);
    matador::access::attribute(op, "flag", flag);
    matador::access::attribute(op, "height", height);
    matador::access::attribute(op, "doubles", doubles);
    matador::access::attribute(op, "bits", bits);
    matador::access::attribute(op, "names", names);
    matador::access::attribute(op, "values", values);
    matador::access::attribute(op, "dimension", dimension);
    matador::access::attribute(op, "dimensions", dimensions);
  }
};

struct base
{
  long id;

  template < class Operator >
  void process(Operator &op)
  {
    matador::access::attribute(op, "id", id);
  }
};

struct derived : public base
{
  std::string name;

  template < class Operator >
  void process(Operator &op)
  {
    matador::access::process(op, *matador::base_class<base>(this));
    matador::access::attribute(op, "name", name);
  }
};

#endif //MATADOR_DTO_HPP
