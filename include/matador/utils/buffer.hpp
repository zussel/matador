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

#include <array>
#include <cstddef>

namespace matador {

class OOS_UTILS_API buffer
{
public:
  buffer() = default;
  buffer(const buffer &x) = default;
  buffer& operator=(const buffer &x) = default;
  ~buffer() = default;

  void append(const char *chunk, std::size_t size);
  void append(const buffer &buf);

  char* data();
  const char* data() const;

  std::size_t capacity() const;
  std::size_t size() const;
  void size(std::size_t s);

  bool empty() const;

  void reset();

private:
  std::array<char, 16384> buf_{};
  std::size_t size_ = 0;
};

}

#endif //MATADOR_BUFFER_HPP
