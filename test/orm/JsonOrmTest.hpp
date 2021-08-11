#ifndef MATADOR_JSONORMTEST_HPP
#define MATADOR_JSONORMTEST_HPP

#include "matador/unit/unit_test.hpp"

class JsonOrmTest : public matador::unit_test
{
public:
  JsonOrmTest(const std::string &prefix, std::string dns);

  void test_insert_from_json();

private:
  std::string dns_;
};


#endif //MATADOR_JSONORMTEST_HPP
