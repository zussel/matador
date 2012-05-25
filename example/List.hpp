#ifndef LIST_HPP
#define LIST_HPP

#include "Command.hpp"

class List : public Command
{
public:
  List();
  virtual ~List();

  virtual void execute(oos::session &db, const std::vector<std::string> &args);
};

#endif /* LIST_HPP */