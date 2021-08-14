#ifndef MATADOR_JSONSERIALIZERTEST_HPP
#define MATADOR_JSONSERIALIZERTEST_HPP

#include "matador/unit/unit_test.hpp"

class JsonSerializerTest : public matador::unit_test
{
public:
  JsonSerializerTest();

  void test_date_to_string_object();
  void test_to_string_object();
  void test_to_string_array();
};


#endif //MATADOR_JSONSERIALIZERTEST_HPP
