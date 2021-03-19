#ifndef MATADOR_TEMPLATE_STATE_HPP
#define MATADOR_TEMPLATE_STATE_HPP

#include <string>

namespace matador {
namespace http {
namespace detail {

class template_state
{
public:
  virtual ~template_state() = default;

  virtual bool is_executable() const = 0;
  virtual void execute() = 0;
  virtual void append(const std::string &part) = 0;
  virtual void append(char c) = 0;
  virtual std::string str() const = 0;
};

class global_state : public template_state
{
public:
  void execute() override;

  bool is_executable() const override;

  void append(const std::string &part) override;

  void append(char c) override;

  std::string str() const override;

private:
  std::string rendered_;
};

class foreach_state : public template_state
{
public:
  bool is_executable() const override;

  void execute() override;

  void append(const std::string &part) override;

  void append(char c) override;

  std::string str() const override;
};

}
}
}
#endif //MATADOR_TEMPLATE_STATE_HPP
