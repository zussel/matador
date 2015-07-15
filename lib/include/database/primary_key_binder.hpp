//
// Created by sascha on 7/13/15.
//

#include "object/object_atomizer.hpp"
#include "object/primary_key.hpp"

#include "database/statement.hpp"

#ifndef PRIMARY_KEY_BINDER_HPP
#define PRIMARY_KEY_BINDER_HPP

namespace oos {

class statement;
class serializable;

class primary_key_binder : public generic_object_reader<primary_key_binder>
{
public:
  primary_key_binder() : generic_object_reader<primary_key_binder>(this) {}
  virtual ~primary_key_binder() {}

  void bind(serializable *obj, statement *stmt, int pos);

  template < class T >
  void read_value(const char*, T &x);
  void read_value(const char*, primary_key_base &x);
  void read_value(const char*, char *, int) {}

private:
  void setup(statement *stmt, serializable *obj, int pos);
  void cleanup();

private:
  bool reading_pk_ = false;
  statement *stmt_ = nullptr;
  int pos_ = 0;
  serializable *obj_ = nullptr;
};

template < class T >
void primary_key_binder::read_value(const char*, T &x) {
  if (reading_pk_) {
    stmt_->bind(pos_, x);
  }
}

}

#endif /* PRIMARY_KEY_BINDER_HPP */

