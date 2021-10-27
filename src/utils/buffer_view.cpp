#include "matador/utils/buffer_view.hpp"

namespace matador {

buffer_view::buffer_view(const char *start, size_t size)
  : start_(start)
  , cursor_(const_cast<char*>(start))
  , size_(size)
{}

buffer_view::buffer_view(const std::string &str)
  : start_(str.data())
  , cursor_(const_cast<char*>(str.data()))
  , size_(str.size())
{}

buffer_view::buffer_view(const char *start)
  : start_(start)
  , cursor_(const_cast<char*>(start))
  , size_(strlen(start))
{}

buffer_view::buffer_view(buffer_view &&x) noexcept
  : start_(x.start_)
  , cursor_(x.cursor_)
  , size_(x.size_)
{}

buffer_view &buffer_view::operator=(buffer_view &&x) noexcept
{
  start_ = x.start_;
  cursor_ = x.cursor_;
  size_ = x.size_;
  return *this;
}

buffer_view &buffer_view::operator=(const std::string &str)
{
  start_ = str.data();
  cursor_ = const_cast<char*>(str.data());
  size_ = str.size();
  return *this;
}

void buffer_view::bump(size_t len)
{
  cursor_ += std::min(len, capacity());
}

const char *buffer_view::data() const
{
  return cursor_;
}

char *buffer_view::data()
{
  return cursor_;
}

size_t buffer_view::size() const
{
  return size_;
}

size_t buffer_view::capacity() const
{
  return size_ - (cursor_ - start_);
}

bool buffer_view::empty() const
{
  return capacity() == size_;
}

bool buffer_view::full() const
{
  return capacity() == 0;
}

void buffer_view::clear()
{
	cursor_ = const_cast<char*>(start_);
}

}