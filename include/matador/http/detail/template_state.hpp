#ifndef MATADOR_TEMPLATE_STATE_HPP
#define MATADOR_TEMPLATE_STATE_HPP

#include <string>

namespace matador {

class string_cursor;
class json;

namespace http {
namespace detail {

class template_state
{
public:
  virtual ~template_state() = default;

  virtual void configure(string_cursor &cursor) = 0;
  virtual bool is_executable() const = 0;
  virtual bool is_end_tag(const std::string &tag) const = 0;
  virtual void execute(const json &data) = 0;
  virtual void append(const std::string &part) = 0;
  virtual void append(char c) = 0;
  virtual std::string str() const = 0;
};

class global_state : public template_state
{
public:
  void execute(const json &data) override;

  void configure(string_cursor &cursor) override;

  bool is_executable() const override;

  bool is_end_tag(const std::string &tag) const override;

  void append(const std::string &part) override;

  void append(char c) override;

  std::string str() const override;

private:
  std::string rendered_;
};

class foreach_state : public template_state
{
public:
  void configure(string_cursor &cursor) override;

  bool is_executable() const override;

  bool is_end_tag(const std::string &tag) const override;

  void execute(const json &data) override;

  void append(const std::string &part) override;

  void append(char c) override;

  std::string str() const override;

private:
  std::string elem_name_;
  std::string list_name_;
};

}
}
}
#endif //MATADOR_TEMPLATE_STATE_HPP
