#include "fs/link.hpp"

namespace oos {
link::link()
{}

link::link(const std::string &linkpath)
  : linkpath_(linkpath)
{}

link::~link()
{}

bool link::operator==(const link &x) const
{
  return linkpath_ == x.linkpath_;
}

bool link::operator!=(const link &x) const
{
  return !this->operator==(x);
}

void link::reset(const char *linkpath)
{
  linkpath_.assign(linkpath);
}

std::string link::name() const
{
  return linkpath_;
}

}
