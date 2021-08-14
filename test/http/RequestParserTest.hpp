#ifndef MATADOR_REQUESTPARSERTEST_HPP
#define MATADOR_REQUESTPARSERTEST_HPP

#include "matador/unit/unit_test.hpp"

class RequestParserTest : public matador::unit_test
{
public:
  RequestParserTest();

  void test_reset_request_parser();
  void test_common_get_request();
  void test_common_get_query_request();
  void test_short_get_request();
  void test_query_post_request();
  void test_post_form_data_request();
  void test_xml_post_request();
  void test_xml_post_partial_request();
  void test_xml_post_partial_request_2();
};


#endif //MATADOR_REQUESTPARSERTEST_HPP
