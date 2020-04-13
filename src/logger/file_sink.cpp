#include "matador/logger/file_sink.hpp"

namespace matador {

file_sink::file_sink(const std::string &path)
: file_sink(path.c_str())
{}

file_sink::file_sink(const char *path)
{
#ifdef _MSC_VER
  fopen_s(&stream, path, "a");
#else
  stream = fopen(path, "a");
#endif
  if (stream == nullptr) {
    throw std::logic_error("error opening file");
  }
}

file_sink::~file_sink()
{
  fclose(stream);
}

stdout_sink::stdout_sink()
  : basic_file_sink(stdout)
{}

stderr_sink::stderr_sink()
  : basic_file_sink(stderr)
{}
}
