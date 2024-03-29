#include "matador/object/action.hpp"
#include "matador/object/object_serializer.hpp"
#include "matador/object/object_deserializer.hpp"

namespace matador {

action::action()
: serializer_(new object_serializer), deserializer_(new object_deserializer) {}

action::~action() {
  delete serializer_;
  delete deserializer_;
}

}