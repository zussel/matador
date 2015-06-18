//
// Created by sascha on 6/18/15.
//

#include "object/primary_key_assigner.hpp"
#include "object/serializable.hpp"

namespace oos {

primary_key_assigner::primary_key_assigner()
  : generic_object_reader<primary_key_assigner>(this)
{ }

primary_key_assigner::~primary_key_assigner()
{ }


void primary_key_assigner::assign(serializable *o, long id)
{
  id_ = id;
  o->deserialize(*this);
  id_ = 0;
}

}