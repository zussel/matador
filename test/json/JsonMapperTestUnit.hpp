//
// Created by sascha on 27.02.20.
//

#ifndef MATADOR_JSONMAPPERTESTUNIT_HPP
#define MATADOR_JSONMAPPERTESTUNIT_HPP

#include "matador/unit/unit_test.hpp"

class JsonMapperTestUnit : public matador::unit_test
{
public:
  JsonMapperTestUnit();

  void test_fields();
  void test_array_of_builtins();
  void test_array_of_objects();
  void test_nested_object();
  void test_nested_array_of_object();
  void test_complex();
  void test_failure();
  void test_false_types();
  void test_special_chars();
  void test_json_to_string();
};


#endif //MATADOR_JSONMAPPERTESTUNIT_HPP
