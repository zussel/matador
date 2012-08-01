#include "json/json_object.hpp"
#include "json/json_string.hpp"

#include <algorithm>

namespace oos {

json_object::json_object()
{}

json_object::~json_object()
{
  clear();
}

bool json_object::parse(std::istream &in)
{
  in >> std::ws;

  /*
   * parse '{'
   * 
   * parse string (key)
   * 
   * parse ':'
   * 
   * parse value (object, string, array, bool or null)
   * 
   * parse '}'
   */
  char c(0);
  
  c = in.get();
  if (in.good() && c != '{') {
    in.putback(c);
    return false;
  }
  
  c = in.get();
  if (in.good() && c == '}') {
    // empty object
    return true;
  }
  
  // skip white
  in >> std::ws;

  do {

    json_string key;
    if (!key.parse(in)) {
      return false;
    }

    // skip ws
    in >> std::ws;

    // read colon
    c = in.get();
    if (in.good() && c != ':') {
      return false;
    }

    // skip ws
    in >> std::ws;

    // create value
    json_value *val = json_value::create(in);

    if (val) {
      in >> *val;
    }

    insert(key, val);

    // skip white
    in >> std::ws;

    c = in.get();
  } while (in && in.good() && c == ',');

  if (c != '}') {
    return false;
  }

  // skip white
  in >> std::ws;

  // no characters after closing parenthesis are aloud
  return in.eof();
}

void json_object::print(std::ostream &out) const
{
  out << "{ ";

  t_string_value_map::const_iterator first = string_value_map_.begin();
  t_string_value_map::const_iterator last = string_value_map_.end();

  while (first != last) {
    out << first->first << " : " << *(first->second);
    if (++first != last) {
      out << ", ";
    }
  }

  out << " }";
}

void delete_json_value(json_object::t_string_value_map::value_type &val)
{
  delete val.second;
}

void json_object::clear()
{
  std::for_each(string_value_map_.begin(), string_value_map_.end(), delete_json_value);
  string_value_map_.clear();
}

bool json_object::empty() const
{
  return string_value_map_.empty();
}

json_object::size_type json_object::size() const
{
  return string_value_map_.size();
}

bool json_object::contains(const std::string &k)
{
  return string_value_map_.count(json_string(k)) > 0;
}

const json_value* json_object::value(const std::string &key) const
{
  t_string_value_map::const_iterator i = string_value_map_.find(json_string(key));
  if (i != string_value_map_.end()) {
    return i->second;
  } else {
    return 0;
  }
}

json_value* json_object::value(const std::string &key)
{
  t_string_value_map::iterator i = string_value_map_.find(json_string(key));
  if (i != string_value_map_.end()) {
    return i->second;
  } else {
    return 0;
  }
}

void json_object::insert(const json_string &key, json_value *val)
{
  string_value_map_.insert(std::make_pair(key, val));
}

}