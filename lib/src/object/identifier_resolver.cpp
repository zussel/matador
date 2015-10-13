//
// Created by sascha on 4/10/15.
//

#include "object/identifier_resolver.hpp"
#include "object/identifier.hpp"
#include "object/object_ptr.hpp"
#include "object/serializable.hpp"

namespace oos {


identifier_resolver::identifier_resolver()
  : generic_object_reader<identifier_resolver>(this)
{}

identifier_resolver::~identifier_resolver()
{}

void identifier_resolver::read_value(const char *, basic_identifier &x)
{
  id_ = x.share();
}

}