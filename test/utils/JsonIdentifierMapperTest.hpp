//
// Created by sascha on 27.01.20.
//

#ifndef MATADOR_JSONIDENTIFIERMAPPERTEST_HPP
#define MATADOR_JSONIDENTIFIERMAPPERTEST_HPP


#include "matador/unit/unit_test.hpp"

#include "matador/utils/json_identifier_mapper.hpp"
#include "matador/utils/identifier.hpp"

class JsonIdentifierMapperTest : public matador::unit_test
{
public:
  JsonIdentifierMapperTest();

  void test_integer_identifier();
  void test_varchar_identifier();

private:
  template < typename T >
  void check_integer_identifier(T result, matador::json_identifier_mapper &id_mapper)
  {
    matador::value_t val((long long)result);
    matador::identifier<T> cid;
    id_mapper.set_identifier_value(cid, &val);
    UNIT_EXPECT_EQUAL(result, cid.value());
  }
};


#endif //MATADOR_JSONIDENTIFIERMAPPERTEST_HPP
