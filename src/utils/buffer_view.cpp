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
  std::lock_guard<std::mutex> lock(x.mutex_);
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
  std::lock_guard<std::mutex> lock(mutex_);
  cursor_ += std::min(len, capacity_unlocked());
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
  std::lock_guard<std::mutex> lock(mutex_);
  return size_ - (cursor_ - start_);
}

bool buffer_view::empty() const
{
  std::lock_guard<std::mutex> lock(mutex_);
  return capacity_unlocked() == size_;
}

bool buffer_view::full() const
{
  std::lock_guard<std::mutex> lock(mutex_);
  return capacity_unlocked() == 0;
}

void buffer_view::clear()
{
  std::lock_guard<std::mutex> lock(mutex_);
	cursor_ = const_cast<char*>(start_);
}

size_t buffer_view::capacity_unlocked() const {
  return size_ - (cursor_ - start_);
}

}