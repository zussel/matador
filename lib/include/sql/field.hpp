//
// Created by sascha on 2/23/16.
//

#ifndef OOS_FIELD_HPP
#define OOS_FIELD_HPP

#ifdef _MSC_VER
#ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4355)
#else
#define OOS_API
#endif

#include "sql/types.hpp"

#include <string>

namespace oos {

class OOS_API field
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

  data_type_t type() const;
  void type(data_type_t t);

  bool is_not_null() const;
  void not_null(bool nn);

  std::string default_value() const;
  void default_value(const std::string &value);

private:
  size_t index_ = 0;
  std::string name_;
  data_type_t type_;
  bool not_null_ = false;
  std::string default_value_;
};

}
#endif //OOS_FIELD_HPP
