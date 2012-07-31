#ifndef JSON_VALUE_HPP
#define JSON_VALUE_HPP

#include <istream>

namespace oos {

class json_value
{
public:
  virtual ~json_value() {}
  
  virtual bool parse(std::istream &in) = 0;
  virtual void print(std::ostream &out) const = 0;

  static json_value* create(std::istream &in);

  friend std::istream& operator>>(std::istream &str, json_value &value);
  friend std::ostream& operator<<(std::ostream &str, const json_value &value);
};

}

#endif /* JSON_VALUE_HPP */