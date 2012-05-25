#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <vector>

namespace oos {
  class session;
}

class Command
{
protected:
  Command(const std::string &name)
    : name_(name)
  {}

public:
  virtual ~Command() {}

  std::string name() const { return name_; }

  virtual void execute(oos::session &db, const std::vector<std::string> &args) = 0;

private:
  std::string name_;
};

#endif /* COMMAND_HPP */
