#include "json/json_array.hpp"

namespace oos {

json_array::json_array()
  : json_type("json_array")
{}

json_array::json_array(size_t size)
  : json_type("json_array")
  , value_vector_(size, json_value())
{}

json_array::json_array(const json_value &x)
  : json_type("json_array")
{
  const json_array *a = x.value_type<json_array>();
  if (a) {
    value_vector_.assign(a->begin(), a->end());
  } else {
    throw std::logic_error("json_value isn't of type json_array");
  }
}

json_array& json_array::operator=(const json_value &x)
{
  const json_array *a = x.value_type<json_array>();
  if (a) {
    value_vector_.clear();
    value_vector_.assign(a->begin(), a->end());
  } else {
    throw std::logic_error("json_value isn't of type json_array");
  }
  return *this;
}

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

    try {
      // create value
      json_value val(json_value::create(in));

      in >> val;

      value_vector_.push_back(val);
    } catch (std::logic_error &ex) {
      return false;
    }

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
    out << *first;
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

bool json_array::empty() const
{
  return value_vector_.empty();
}

void json_array::clear()
{
  value_vector_.clear();
}

json_value& json_array::operator[](size_t index)
{
  return value_vector_[index];
}

const json_value& json_array::operator[](size_t index) const
{
  return value_vector_[index];
}

void json_array::push_back(const json_value &x)
{
  value_vector_.push_back(x);
}

size_t json_array::size() const
{
  return value_vector_.size();
}

std::istream& operator>>(std::istream &str, json_array &ary)
{
  ary = json_type::parser.parse(str);
  return str;
}

}
