#include "object/attribute_serializer.hpp"

namespace oos {

void convert(const char* from, char *to, int size, int)
{
  strncpy(to, from, size);
}

void convert(const char* from, char *to, int size)
{
  strncpy(to, from, size);
}

}
