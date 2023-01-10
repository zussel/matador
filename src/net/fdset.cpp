#include "matador/net/fdset.hpp"

namespace matador {

fdset::fdset()
{
  reset();
}

fdset::fdset(fdset&& x) noexcept
: max_fd_set_(std::move(x.max_fd_set_))
, fd_set_(x.fd_set_) {}

fdset& fdset::operator=(fdset&& x) noexcept
{
  max_fd_set_ = std::move(x.max_fd_set_);
  fd_set_ = x.fd_set_;
  return *this;
}

// set all bits to zero
void fdset::reset()
{
  FD_ZERO(&fd_set_);
  max_fd_set_.clear();
  max_fd_set_.insert(0);
}

bool fdset::is_set(int fd) const
{
  return FD_ISSET(fd, &fd_set_) > 0;
}

void fdset::clear(int fd)
{
  FD_CLR(fd, &fd_set_);
  max_fd_set_.erase(fd);
}

void fdset::set(int fd)
{
  FD_SET(fd, &fd_set_);
  max_fd_set_.insert(fd);
}

int fdset::maxp1() const
{
  return *max_fd_set_.begin();
}

size_t fdset::count() const
{
  return max_fd_set_.size() - 1;
}

bool fdset::empty() const
{
  return count() == 0;
}

fd_set* fdset::get()
{
  return &fd_set_;
}

}
