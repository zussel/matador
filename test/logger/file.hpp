#ifndef MATADOR_FILEHELPER_HPP
#define MATADOR_FILEHELPER_HPP

#include <string>

#ifdef _MSC_VER
#include <io.h>
#else
#include <unistd.h>
#endif

namespace filehelper {

class std_stream_switcher
{
public:
  explicit std_stream_switcher(FILE *str, const char* redirect);
  ~std_stream_switcher();

private:
  FILE *stream = nullptr;
  int fd = 0;
  fpos_t pos = {};
};

}

#endif //MATADOR_FILEHELPER_HPP
