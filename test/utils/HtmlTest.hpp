#ifndef MATADOR_HTMLTEST_HPP
#define MATADOR_HTMLTEST_HPP

#include "matador/unit/unit_test.hpp"

class HtmlTest : public matador::unit_test
{
public:
  HtmlTest();

  void test_escape();
};


#endif //MATADOR_HTMLTEST_HPP
