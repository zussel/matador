#ifndef MATADOR_STRING_CURSOR_HPP
#define MATADOR_STRING_CURSOR_HPP

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

namespace matador {

class OOS_UTILS_API string_cursor
{
public:
  explicit string_cursor(const char *str);
  string_cursor& operator=(const char *str);

  char operator[](int i) const { return cursor_[i]; }
  bool is_null() const { return cursor_ == nullptr; }
  const char* operator()() const { return cursor_; }
  void sync_cursor(const char *cursor) { cursor_ = cursor; }

  char skip_whitespace();

  char next_char();

  char current_char() const;

private:
  const char *cursor_ = nullptr;
};

}

#endif //MATADOR_STRING_CURSOR_HPP
