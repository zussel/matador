#ifndef MATADOR_BUFFER_HPP
#define MATADOR_BUFFER_HPP

#include <cstddef>

namespace matador {

class buffer
{
public:
  buffer() = delete;

public:
  buffer(const buffer &x) = default;
  buffer& operator=(const buffer &x) = default;
  buffer(char *buf, std::size_t capacity);
  ~buffer();

  void append(const char *chunk, std::size_t size);
  void append(const buffer &buf);

  char* data();
  const char* data() const;

  std::size_t capacity() const;
  std::size_t size() const;

private:
  char *buf_;
  std::size_t size_;
  std::size_t capacity_;
};

}

#endif //MATADOR_BUFFER_HPP
