#ifndef MATADOR_IDENTIFIERSERIALIZERTEST_H
#define MATADOR_IDENTIFIERSERIALIZERTEST_H

#include "matador/unit/unit_test.hpp"

namespace matador {
class connection;
}

class IdentifierSerializerTest  : public matador::unit_test {
public:
  IdentifierSerializerTest(const std::string &prefix, std::string dns);

  void test_identifier_result_test();
  void test_identifier_row_result_test();
  void test_identifier_statement_test();

private:
  template < typename IdType, typename SecondIdType, typename ... OtherIdTypes >
  void test_identifier_result(matador::connection &conn);
  template < typename IdType >
  void test_identifier_result(matador::connection &conn);

  template < typename IdType, typename SecondIdType, typename ... OtherIdTypes >
  void test_identifier_row_result(matador::connection &conn);
  template < typename IdType >
  void test_identifier_row_result(matador::connection &conn);

private:
  std::string dns_;
};


#endif //MATADOR_IDENTIFIERSERIALIZERTEST_H
