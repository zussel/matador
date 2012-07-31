#ifndef JSON_BOOL_HPP
#define JSON_BOOL_HPP

#include "json/json_value.hpp"

#include <iostream>

namespace oos {

class json_bool : public json_value
{
public:
  json_bool(void);
  explicit json_bool(bool val);
  virtual ~json_bool(void);

  virtual bool parse(std::istream &in);
  virtual void print(std::ostream &out) const;

  bool value() const;
  void value(bool val);

private:
  static const char *true_string;
  static const char *false_string;

  bool value_;
};

}

#endif /* JSON_BOOL_HPP */