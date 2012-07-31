#ifndef JSON_STRING_HPP
#define JSON_STRING_HPP

#include "json/json_value.hpp"

#include <string>
#include <iostream>

namespace oos {

class json_string : public json_value
{
public:
  json_string();
  explicit json_string(const std::string &val);
  json_string(const json_string &x);
  json_string& operator=(const json_string &x);
  json_string& operator=(const std::string &x);
  virtual ~json_string();
 
  bool operator<(const json_string &x) const;

  virtual bool parse(std::istream &in);
  virtual void print(std::ostream &out) const;

  std::string value() const;
  void value(const std::string &val);

  void push_back(char c);

private:
  std::string value_;
};

}

#endif /* JSON_STRING_HPP */