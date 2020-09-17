#include <stdexcept>
#include <cstring>
#include "matador/utils/buffer.hpp"

namespace matador {

using namespace std;

void buffer::append(const char *chunk, size_t size)
{
  if (size_ + size > buf_.max_size()) {
    throw std::out_of_range("size exceeds buffer capacity");
  }
  memcpy((void*)(buf_.data() + size_), chunk, size);
  size_ += size;
}

void buffer::append(const buffer &buf)
{
  append(buf.data(), buf.size());
}

char* buffer::data()
{
  return buf_.data();
}

const char* buffer::data() const
{
  return buf_.data();
}

size_t buffer::capacity() const
{
  return buf_.max_size();
}

size_t buffer::size() const
{
  return size_;
}

bool buffer::empty() const
{
  return size_ == 0;
}

void buffer::reset()
{
  size_ = 0;
}
}
