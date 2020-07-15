#include "matador/net/select_fdsets.hpp"

#include <algorithm>

namespace matador {

int select_fdsets::maxp1() const
{
  return (std::max)(fdsets_[0].maxp1(), (std::max)(fdsets_[1].maxp1(), fdsets_[2].maxp1()));
}

fdset& select_fdsets::fd_set(fdset_type type)
{
  return fdsets_[type];
}

fdset& select_fdsets::read_set()
{
  return fdsets_[0];
}

const fdset& select_fdsets::read_set() const
{
  return fdsets_[0];
}

fdset& select_fdsets::write_set()
{
  return fdsets_[1];
}

const fdset& select_fdsets::write_set() const
{
  return fdsets_[1];
}

fdset& select_fdsets::except_set()
{
  return fdsets_[2];
}

const fdset& select_fdsets::except_set() const
{
  return fdsets_[2];
}

void select_fdsets::reset()
{
  for (auto &fdset : fdsets_) {
    fdset.reset();
  }
}

void select_fdsets::reset(fdset_type type)
{
  fdsets_[type].reset();
}

bool select_fdsets::is_set(fdset_type type, int fd) const
{
  return fdsets_[type].is_set(fd);
}

void select_fdsets::clear(fdset_type type, int fd)
{
  fdsets_[type].clear(fd);
}

void select_fdsets::set(fdset_type type, int fd)
{
  fdsets_[type].set(fd);
}

}
