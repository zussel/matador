#ifndef MATADOR_BLOGUNITTEST_HPP
#define MATADOR_BLOGUNITTEST_HPP

#include "matador/unit/unit_test.hpp"

#include <matador/orm/persistence.hpp>

class BlogUnitTest : public matador::unit_test
{
public:
  BlogUnitTest(const std::string& prefix, const std::string& dns);
  ~BlogUnitTest() override = default;

  void finalize() override;

  void test_blog_single_post();
  void test_blog_multiple_post();

private:
  matador::persistence persistence_;
};


#endif //MATADOR_BLOGUNITTEST_HPP
