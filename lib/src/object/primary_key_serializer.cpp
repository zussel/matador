//
// Created by sascha on 4/10/15.
//

#include "object/primary_key_serializer.hpp"
#include "object/primary_key.hpp"
#include "object/object_ptr.hpp"
#include "object/object.hpp"

namespace oos {


primary_key_serializer::primary_key_serializer()
  : generic_object_writer<primary_key_serializer>(this)
{}

primary_key_serializer::~primary_key_serializer()
{}

primary_key_base* primary_key_serializer::serialize(const object *o)
{
  primary_key_ = 0;
  o->serialize(*this);
  return primary_key_;
}

void primary_key_serializer::write_value(const char *, const primary_key_base &x)
{
  primary_key_ = x.clone();
}

}