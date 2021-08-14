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
  void test_foreach_empty();
  void test_if_else();
  void test_include();
  void test_filter();
};


#endif //MATADOR_TEMPLATEENGINETEST_HPP
