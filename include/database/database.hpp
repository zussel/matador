#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <string>

namespace oos {

class database_impl;

class database
{
public:
  database(const std::string &dbstring);
  ~database();
  
  void open();
  void close();

private:
  database_impl *impl_;
};

}

#endif /* DATABASE_HPP */
