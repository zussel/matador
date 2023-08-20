#ifndef SESSION_TEST_UNIT_HPP
#define SESSION_TEST_UNIT_HPP

#include "matador/unit/unit_test.hpp"

namespace matador {
class session;
}

class ConnectionTestUnit : public matador::unit_test
{
public:
  ConnectionTestUnit(const std::string &prefix, std::string dns);
  ~ConnectionTestUnit() override = default;

  void test_open_close();
  void test_reopen();
  void test_reconnect();
  void test_connection_failed();

private:
  std::string dns_;
  std::string db_vendor_;
};

#endif /* SESSION_TEST_UNIT_HPP */