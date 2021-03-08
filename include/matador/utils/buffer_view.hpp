#ifndef MATADOR_BUFFER_VIEW_HPP
#define MATADOR_BUFFER_VIEW_HPP

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

#include <cstring>
#include <string>
#include <array>

namespace matador {

class OOS_UTILS_API buffer_view
{
public:
  buffer_view() = default;
  buffer_view(const char *start, size_t size);
  explicit buffer_view(const char *start);
  explicit buffer_view(const std::string &str);
  template<std::size_t S>
  explicit buffer_view(const std::array<char, S> &ar)
    : start_(ar.data())
    , cursor_(const_cast<char*>(ar.data()))
    , size_(S)
  {}
  buffer_view(buffer_view &&x) noexcept;
  buffer_view(const buffer_view &x) = default;
  buffer_view& operator=(buffer_view &&x) noexcept;
  buffer_view& operator=(const buffer_view &x) = default;
  buffer_view& operator=(const std::string &str);

  void bump(size_t len);

  const char* data() const;
  char* data();

  size_t size() const;
  size_t capacity() const;

  bool empty() const;
  bool full() const;

private:
  const char *start_ = nullptr;
  char *cursor_ = nullptr;
  size_t size_ = 0;
};

}
#endif //MATADOR_BUFFER_VIEW_HPP
