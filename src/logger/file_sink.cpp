#include <cstring>
#include <vector>
#include <matador/utils/string.hpp>
#include "matador/logger/file_sink.hpp"

#include "matador/utils/os.hpp"

namespace matador {

file_sink::file_sink(const std::string &path)
  : path_(path)
{
  std::string filename(path);
  // find last dir delimiter
  const char *last = strrchr(path.c_str(), matador::os::DIR_SEPARATOR);
  if (last != nullptr) {
    path_.assign(path.data(), last-path.data());
  } else {
    path_.clear();
  }

  if (last != nullptr) {
    filename = (last + 1);
  }
  // extract base path and extension
  std::vector<std::string> result;
  if (matador::split(filename, '.', result) != 2) {
    throw std::logic_error("splitted path must consists of two elements");
  }
  // get current path
  auto pwd = os::get_current_dir();
  // make path
  os::mkpath(path_);
  // change into path
  os::chdir(path_);
  // create file
  stream = os::fopen(filename, "a");
  if (stream == nullptr) {
    os::chdir(pwd);
    throw std::logic_error("error opening file");
  }
  os::chdir(pwd);
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
