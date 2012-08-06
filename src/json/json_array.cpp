#include "json/json_array.hpp"

namespace oos {

json_array::json_array()
{}

json_array::~json_array()
{}

bool json_array::parse(std::istream &in)
{
  // skip white
  in >> std::ws;

  char c(0);
  
  c = in.get();
  if (in.good() && c != '[') {
    in.putback(c);
    return false;
  }
  
  c = in.get();
  if (in.good() && c == ']') {
    // empty array
    return true;
  }
  
  // skip white
  in >> std::ws;

  do {

    // skip ws
    in >> std::ws;

    // create value
    json_value *val = json_value::create(in);

    if (val) {
      in >> *val;
    }

    value_vector_.push_back(val);

    // skip white
    in >> std::ws;

    c = in.get();
  } while (in && in.good() && c == ',');

  if (c != ']') {
    return false;
  }

  return true;
}

void json_array::print(std::ostream &out) const
{
  out << "[ ";

  t_value_vector::const_iterator first = value_vector_.begin();
  t_value_vector::const_iterator last = value_vector_.end();

  while (first != last) {
    out << *(*first);
    if (++first != last) {
      out << ", ";
    }
  }

  out << " ]";
}

json_array::iterator json_array::begin()
{
  return value_vector_.begin();
}

json_array::const_iterator json_array::begin() const
{
  return value_vector_.begin();
}

json_array::iterator json_array::end()
{
  return value_vector_.end();
}

json_array::const_iterator json_array::end() const
{
  return value_vector_.end();
}

json_array::size_type json_array::size() const
{
  return value_vector_.size();
}

bool json_array::empty() const
{
  return value_vector_.empty();
}

}
