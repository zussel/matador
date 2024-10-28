#include "mysql_result_info.hpp"

#include <cstring>

namespace matador::backends::mysql {

mysql_result_info::mysql_result_info(size_t size)
: buffer(size)
, buffer_size(size)
{
  memset(buffer.data(), 0, size);
}

void mysql_result_info::initialize(size_t size)
{
  if (size > buffer_size) {
    buffer.resize(size);
    buffer_size = size;
  }
}

}
