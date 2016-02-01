//
// Created by sascha on 2/1/16.
//
#include "tools/serializer.hpp"

namespace oos {

void serializer::serialize(const char *id, oos::basic_identifier &x)
{
  x.serialize(id, *this);
}

}