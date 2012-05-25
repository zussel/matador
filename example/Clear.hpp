#ifndef CLEAR_HPP
#define CLEAR_HPP

#include "Command.hpp"

class Clear : public Command
{
public:
  Clear();
  virtual ~Clear();

  virtual void execute(oos::session &db, const std::vector<std::string> &args);
};

#endif /* CLEAR_HPP */