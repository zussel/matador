#ifndef MATADOR_TEMPLATEENGINETEST_HPP
#define MATADOR_TEMPLATEENGINETEST_HPP

#include "matador/unit/unit_test.hpp"

class TemplateEngineTest : public matador::unit_test
{
public:
  TemplateEngineTest();

  void test_replace_var();
  void test_foreach();
  void test_foreach_nested();
};


#endif //MATADOR_TEMPLATEENGINETEST_HPP
