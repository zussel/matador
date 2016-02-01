//
// Created by sascha on 7/13/15.
//

#include "tools/identifier.hpp"

#include "database/statement.hpp"

#ifndef PRIMARY_KEY_BINDER_HPP
#define PRIMARY_KEY_BINDER_HPP

namespace oos {

template < class T > class statement;
class serializable;

/// @cond OOS_DEV

class identifier_binder
{
public:
  identifier_binder() {}
  virtual ~identifier_binder() {}

  void bind(serializable *obj, statement<serializable> *stmt, int pos);

  template < class T >
  void read_value(const char*, T &x);
  void read_value(const char*, basic_identifier &x);
  void read_value(const char*, char *, size_t) {}

private:
  void setup(statement<serializable> *stmt, serializable *obj, int pos);
  void cleanup();

private:
  bool reading_pk_ = false;
  statement<serializable> *stmt_ = nullptr;
  int pos_ = 0;
  serializable *obj_ = nullptr;
};

template < class T >
void identifier_binder::read_value(const char*, T &x) {
  if (reading_pk_) {
    stmt_->bind(pos_, x);
  }
}

/// @endcond

}

#endif /* PRIMARY_KEY_BINDER_HPP */

