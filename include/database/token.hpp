#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "database/sql.hpp"
#include "database/condition.hpp"

namespace oos {

/// @cond OOS_DEV

struct string_token : public sql::token {
  string_token() {}
  string_token(const std::string &s) : str(s) {}

  virtual std::string get(bool) const {
    return str;
  }

  std::string str;
};

struct result_field_token : public sql::token {
  result_field_token(sql::field_ptr f) : fld(f) {}

  virtual std::string get(bool prepared) const {
    if (prepared) {
      return fld->name;
    } else {
      return fld->name;
    }
  }

  sql::field_ptr fld;
};

struct condition_token : public sql::token
{
  condition_token(const condition &c)
    : cond(c)
  {}
  
  virtual std::string get(bool prepared) const {
    return cond.str(prepared);
  }

  condition cond;
};

struct host_field_token : public string_token {
  host_field_token(sql::field_ptr f) : fld(f) {}
  host_field_token(sql::field_ptr f, const std::string &s)
    : string_token(s), fld(f)
  {}

  virtual std::string get(bool prepared) const {
    if (prepared) {
      return std::string("?");
    } else {
      return str;
    }
  }

  sql::field_ptr fld;
};

/// @endcond

}

#endif /* TOKEN_HPP */
