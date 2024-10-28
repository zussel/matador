#ifndef MATADOR_MYSQL_RESULT_INFO_HPP
#define MATADOR_MYSQL_RESULT_INFO_HPP

#include "mysql_bool.hpp"

#include <vector>

namespace matador::backends::mysql {

struct mysql_result_info
{
  mysql_result_info() = default;
  explicit mysql_result_info(size_t size);

  void initialize(size_t size);

  unsigned long length{0};
  my_bool is_null = false;
  my_bool error = false;
  std::vector<unsigned char> buffer;
  size_t buffer_size{};
};

}

#endif //MATADOR_MYSQL_RESULT_INFO_HPP
