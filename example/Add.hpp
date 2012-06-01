#ifndef ADD_HPP
#define ADD_HPP

#include "Command.hpp"

class Add : public Command
{
public:
  Add();
  virtual ~Add();

  virtual void execute(oos::session &db, const std::vector<std::string> &args);
};

#endif /* ADD_HPP */
