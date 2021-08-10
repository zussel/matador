#ifndef MATADOR_JSONOBJECTMAPPERTEST_HPP
#define MATADOR_JSONOBJECTMAPPERTEST_HPP


#include "matador/unit/unit_test.hpp"

class JsonObjectMapperTest : public matador::unit_test
{
public:
  JsonObjectMapperTest();

  void test_simple();
  void test_derived();
  void test_has_many();
  void test_has_many_builtin();
  void test_array();
  void test_to_json();
  void test_to_string();
  void test_to_ptr();
};


#endif //MATADOR_JSONOBJECTMAPPERTEST_HPP
