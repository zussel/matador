//
// Created by sascha on 7/13/15.
//
#include "database/identifier_binder.hpp"

#include "object/serializable.hpp"

namespace oos {

void identifier_binder::bind(serializable *obj, statement<serializable> *stmt, int pos)
{
  setup(stmt, obj, pos);

  obj->deserialize(*this);

  cleanup();
}

void identifier_binder::read_value(const char *id, basic_identifier &x)
{
  reading_pk_ = true;
  x.deserialize(id, *this);
  reading_pk_ = false;
}

void identifier_binder::setup(statement<serializable> *stmt, serializable *obj, int pos)
{
  stmt_ = stmt;
  pos_ = pos;
  obj_ = obj;
}

void identifier_binder::cleanup()
{
  stmt_ = nullptr;
  pos_ = 0;
  obj_ = nullptr;
}

}