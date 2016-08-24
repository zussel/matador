//
// Created by sascha on 2/23/16.
//

#ifndef OOS_FIELD_HPP
#define OOS_FIELD_HPP

#include "sql/types.hpp"

#include <string>

namespace oos {

class field
{
public:
  field();
  field(const char *name);
  field(const std::string &name);
  ~field();

  size_t index() const;
  void index(size_t i);

  std::string name() const;
  void name(const std::string &n);

//  std::string type() const;
//  void type(const std::string &t);
  data_type_t type() const;
  void type(data_type_t t);

  bool is_not_null() const;
  void not_null(bool nn);

  std::string default_value() const;
  void default_value(const std::string &value);

//  bool is_primary_key() const;
//  void is_primary_key(bool is_pk);
//
private:
  size_t index_ = 0;
  std::string name_;
//  std::string type_;
  data_type_t type_;
  bool not_null_ = false;
  std::string default_value_;
//  bool is_primary_key_;
};

}
#endif //OOS_FIELD_HPP
