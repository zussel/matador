#ifndef MATADOR_TEMPLATE_ENGINE_HPP
#define MATADOR_TEMPLATE_ENGINE_HPP

#include <string>
#include <stack>
#include <memory>

namespace matador {

class json;

namespace http {

namespace detail {

class template_state;

}
class string_cursor
{
public:
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

class template_engine
{
public:
  void render(const std::string &format, const matador::json &data);

  void render(const char *format, size_t len, const matador::json &data);

  const std::string& str() const;

private:
  void handle_variable(const json &data);
  void handle_command(const json &data);
  std::string parse_token();

private:
  std::string rendered_;

  string_cursor cursor_;

  std::stack<std::shared_ptr<detail::template_state>> state_stack_;
};

}
}
#endif //MATADOR_TEMPLATE_ENGINE_HPP
