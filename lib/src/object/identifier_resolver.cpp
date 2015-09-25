//
// Created by sascha on 4/10/15.
//

#include "object/identifier_resolver.hpp"
#include "object/identifier.hpp"
#include "object/object_ptr.hpp"
#include "object/serializable.hpp"

namespace oos {


identifier_resolver::identifier_resolver()
  : generic_object_writer<identifier_resolver>(this)
{}

identifier_resolver::~identifier_resolver()
{}

void identifier_resolver::write_value(const char *, const basic_identifier &x)
{
  id_ = x.clone();
}

}