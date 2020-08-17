#ifndef MATADOR_BUFFER_HPP
#define MATADOR_BUFFER_HPP

#ifdef _MSC_VER
  #ifdef matador_utils_EXPORTS
    #define OOS_UTILS_API __declspec(dllexport)
    #define EXPIMP_UTILS_TEMPLATE
  #else
    #define OOS_UTILS_API __declspec(dllimport)
    #define EXPIMP_UTILS_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
  #define OOS_UTILS_API
#endif

#include <cstddef>

namespace matador {

class OOS_UTILS_API buffer
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

  void reset();

private:
  char *buf_;
  std::size_t size_;
  std::size_t capacity_;
};

}

#endif //MATADOR_BUFFER_HPP
