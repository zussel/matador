#ifndef MATADOR_VERSIONTEST_HPP
#define MATADOR_VERSIONTEST_HPP

#include "matador/unit/unit_test.hpp"

class VersionTest : public matador::unit_test
{
public:
  VersionTest();

  void test_version();
  void test_parse();
};


#endif //MATADOR_VERSIONTEST_HPP