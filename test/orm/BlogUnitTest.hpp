//
// Created by sascha on 22.12.17.
//

#ifndef MATADOR_BLOGUNITTEST_HPP
#define MATADOR_BLOGUNITTEST_HPP

#include "matador/unit/unit_test.hpp"

class BlogUnitTest : public matador::unit_test
{
public:
  BlogUnitTest(const std::string &prefix, const std::string &dns);
  ~BlogUnitTest() override = default;

  void test_blog();

private:
  std::string dns_;
};


#endif //MATADOR_BLOGUNITTEST_HPP
