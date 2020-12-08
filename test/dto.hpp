#ifndef MATADOR_DTO_HPP
#define MATADOR_DTO_HPP

#include "matador/utils/varchar.hpp"
#include "matador/utils/identifier.hpp"
#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"

#include <set>
#include <unordered_set>

struct bounding_box
{
  long length = 0;
  long width = 0;
  long height = 0;

  template < class S >
  void serialize(S &s)
  {
    s.serialize("length", length);
    s.serialize("width", width);
    s.serialize("height", height);
  }
};

struct dto
{
  matador::identifier<matador::varchar<255>> id;
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
    s.serialize("id", id);
    s.serialize("name", name);
    s.serialize("birthday", birthday);
    s.serialize("created", created);
    s.serialize("flag", flag);
    s.serialize("height", height);
    s.serialize("doubles", doubles);
    s.serialize("bits", bits);
    s.serialize("names", names);
    s.serialize("values", values);
    s.serialize("dimension", dimension);
    s.serialize("dimensions", dimensions);
  }
};

#endif //MATADOR_DTO_HPP
