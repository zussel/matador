#include "matador/utils/value_type_traits.hpp"

#include "matador/utils/attribute_reader.hpp"
#include "matador/utils/attribute_writer.hpp"

namespace matador::utils {

void data_type_traits<value>::read_value(attribute_reader &reader, const char *id, size_t index, value &val)
{
  reader.read_value(id, index, val, val.size());
}

void data_type_traits<value>::bind_value(attribute_writer &binder, size_t index, value &val, size_t size)
{
  binder.write_value(index, val, size);
}

}