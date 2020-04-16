#include "matador/logger/file_sink.hpp"

namespace matador {

file_sink::file_sink(const std::string &path)
  : path_(path)
{
#ifdef _MSC_VER
  fopen_s(&stream, path, "a");
#else
  stream = fopen(path.c_str(), "a");
#endif
  if (stream == nullptr) {
    throw std::logic_error("error opening file");
  }
}

file_sink::file_sink(const char *path)
  : file_sink(std::string(path))
{}

file_sink::~file_sink()
{
  if (stream) {
    fclose(stream);
    stream = nullptr;
  }
}

std::string file_sink::path() const
{
  return path_;
}

stdout_sink::stdout_sink()
  : basic_file_sink(stdout)
{}

stderr_sink::stderr_sink()
  : basic_file_sink(stderr)
{}
}
