#ifndef MATADOR_STREAMSTEST_HPP
#define MATADOR_STREAMSTEST_HPP

#include "matador/unit/unit_test.hpp"

class StreamsTest : public matador::unit_test
{
public:
  StreamsTest();

  void test_generate();
  void test_iterate();
  void test_min();
  void test_max();
  void test_filter();
  void test_map();
  void test_flatmap();
  void test_take();
  void test_take_while();
  void test_skip();
  void test_skip_while();
  void test_every();
  void test_peek();
  void test_concat();
  void test_pack_every();
  void test_first();
  void test_last();
  void test_at();
  void test_any();
  void test_all();
  void test_none();
  void test_count();
  void test_reduce();
  void test_join();
};


#endif //MATADOR_STREAMSTEST_HPP
