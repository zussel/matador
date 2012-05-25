#ifndef CREATE_HPP
#define CREATE_HPP

#include "Command.hpp"

class Create : public Command
{
public:
  Create();
  virtual ~Create();

  virtual void execute(oos::session &db, const std::vector<std::string> &args);
};

#endif /* CREATE_HPP */