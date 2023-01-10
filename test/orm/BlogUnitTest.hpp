#ifndef MATADOR_BLOGUNITTEST_HPP
#define MATADOR_BLOGUNITTEST_HPP

#include "matador/unit/unit_test.hpp"

#include <matador/orm/persistence.hpp>

class BlogUnitTest : public matador::unit_test
{
public:
  BlogUnitTest(const std::string& prefix, std::string  dns);
  ~BlogUnitTest() override = default;

  void initialize() override;
  void finalize() override;

  void test_blog_single_post();
  void test_blog_multiple_post();

private:
  std::string dns_;

  std::unique_ptr<matador::persistence> persistence_;
};


#endif //MATADOR_BLOGUNITTEST_HPP
