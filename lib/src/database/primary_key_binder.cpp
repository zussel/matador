//
// Created by sascha on 7/13/15.
//
#include "database/primary_key_binder.hpp"

#include "object/serializable.hpp"

namespace oos {

void primary_key_binder::bind(serializable *obj, statement *stmt, int pos)
{
  setup(stmt, obj, pos);

  obj->deserialize(*this);

  cleanup();
}

void primary_key_binder::read_value(const char *id, basic_identifier &x)
{
  reading_pk_ = true;
  x.deserialize(id, *this);
  reading_pk_ = false;
}

void primary_key_binder::setup(statement *stmt, serializable *obj, int pos)
{
  stmt_ = stmt;
  pos_ = pos;
  obj_ = obj;
}

void primary_key_binder::cleanup()
{
  stmt_ = nullptr;
  pos_ = 0;
  obj_ = nullptr;
}

}