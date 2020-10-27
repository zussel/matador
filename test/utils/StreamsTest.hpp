#ifndef MATADOR_STREAMSTEST_HPP
#define MATADOR_STREAMSTEST_HPP

#include "matador/unit/unit_test.hpp"

class StreamsTest : public matador::unit_test
{
public:
  StreamsTest();

  void test_iterate();
  void test_min();
  void test_filter();
  void test_map();
  void test_take();
  void test_take_while();
  void test_skip();
  void test_skip_while();
  void test_every();
  void test_peek();
  void test_first();
  void test_last();
  void test_at();
};


#endif //MATADOR_STREAMSTEST_HPP
