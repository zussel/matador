#include <stdexcept>
#include <cstring>
#include "matador/utils/buffer.hpp"

namespace matador {

using namespace std;

buffer::buffer(char *buf, size_t capacity)
  : buf_(buf)
  , size_(0)
  , capacity_(capacity)
{}

buffer::~buffer()
{}

void buffer::append(const char *chunk, size_t size)
{
  if (size_ + size > capacity_) {
    throw std::out_of_range("size exceeds buffer capacity");
  }
  memcpy((void*)(buf_ + size_), chunk, size);
  size_ += size;
}

void buffer::append(const buffer &buf)
{
  append(buf.data(), buf.size());
}

char* buffer::data()
{
  return buf_;
}

const char* buffer::data() const
{
  return buf_;
}

size_t buffer::capacity() const
{
  return capacity_;
}

size_t buffer::size() const
{
  return size_;
}

void buffer::reset()
{
  size_ = 0;
}
}
