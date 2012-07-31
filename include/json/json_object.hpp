#ifndef JSON_OBJECT_HPP
#define JSON_OBJECT_HPP

#include "json/json_value.hpp"
#include "json/json_string.hpp"

#include <iostream>
#include <string>
#include <map>

namespace oos {

class json_object : public json_value
{
public:
  typedef std::map<json_string, json_value*> t_string_value_map;
  typedef t_string_value_map::const_iterator const_iterator;
  typedef t_string_value_map::iterator iterator;
  typedef t_string_value_map::size_type size_type;

public:
  json_object();
  virtual ~json_object();

  virtual bool parse(std::istream &in);
  virtual void print(std::ostream &out) const;

  void clear();
  bool empty() const;
  size_type size() const;

  bool contains(const std::string &k);

  const json_value* value(const std::string &key) const;
  json_value* value(const std::string &key);
  void insert(const json_string &key, json_value *val);

private:
  t_string_value_map string_value_map_;
};

}

#endif /* JSON_OBJECT_HPP */
