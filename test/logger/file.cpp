#include "file.hpp"

#include "matador/utils/os.hpp"

namespace filehelper {

std_stream_switcher::std_stream_switcher(FILE *str, const char *redirect)
  : stream(str)
{
  fflush(stream);
  fgetpos(stream, &pos);
  fd = matador::os::dup(stream);
  matador::os::freopen(redirect, "w+", stream);
}

std_stream_switcher::~std_stream_switcher()
{
  fflush(stream);
#ifdef _MSC_VER
  _dup2(fd, _fileno(stream));
  _close(fd);
#else
  dup2(fd, fileno(stream));
  close(fd);
#endif
  clearerr(stream);
  fsetpos(stream, &pos);
}

}