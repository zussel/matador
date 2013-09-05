#include "fs/file.hpp"

namespace oos {
file::file()
{}

file::file(const std::string &filepath)
  : filepath_(filepath)
{}

file::~file()
{}

bool file::operator==(const file &x) const
{
  return filepath_ == x.filepath_;
}

bool file::operator!=(const file &x) const
{
  return !this->operator==(x);
}

void file::reset(const char *filepath)
{
  filepath_.assign(filepath);
}

std::string file::name() const
{
  return filepath_;
}

}
