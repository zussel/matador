#include "matador/logger/file_sink.hpp"

namespace matador {

file_sink::file_sink(const std::string &path)
: file_sink(path.c_str())
{}

file_sink::file_sink(const char *path)
{
  stream = fopen(path, "a");
  if (stream == nullptr) {
    throw std::logic_error("error opening file");
  }
}

file_sink::~file_sink()
{
  fclose(stream);
}

}
