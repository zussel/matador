#include "RequestParserTest.hpp"
#include "RequestData.hpp"

#include "matador/http/request_parser.hpp"
#include "matador/http/request_header.hpp"
#include "matador/http/request.hpp"
#include "matador/http/mime_types.hpp"

using namespace matador::http;

RequestParserTest::RequestParserTest()
  : matador::unit_test("request_parser", "request parser test")
{
  add_test("reset", [this] { test_reset_request_parser(); }, "test reset request parser");
  add_test("get_common", [this] { test_common_get_request(); }, "parse common get request");
  add_test("get_common_query", [this] { test_common_get_query_request(); }, "parse common get query request");
  add_test("get_short", [this] { test_short_get_request(); }, "parse short get request");
  add_test("post_query", [this] { test_query_post_request(); }, "parse post query request");
  add_test("post_form_data", [this] { test_post_form_data_request(); }, "parse post form data request");
  add_test("post_xml", [this] { test_xml_post_request(); }, "parse post xml request");
  add_test("post_xml_partial", [this] { test_xml_post_partial_request(); }, "parse post xml partial request");
  add_test("post_xml_partial_2", [this] { test_xml_post_partial_request_2(); }, "parse post xml partial request 2");
}

void RequestParserTest::test_reset_request_parser()
{
  request_parser parser;
  request req1, req2;
  /*
   * read in partial request
   * reset parser and read in
   * complete request
   */
  UNIT_ASSERT_EQUAL(request_parser::PARTIAL, parser.parse(RequestData::POST_PARTIAL_BEGIN, req1));

  parser.reset();

  size_t expected_header_size(1);

  UNIT_ASSERT_EQUAL(request_parser::FINISH, parser.parse(RequestData::GET_SHORT, req2));
  UNIT_ASSERT_EQUAL(http::GET, req2.method());
  UNIT_ASSERT_EQUAL("/api", req2.url());
  UNIT_ASSERT_EQUAL(1, req2.version().major);
  UNIT_ASSERT_EQUAL(1, req2.version().minor);
  UNIT_ASSERT_EQUAL(expected_header_size, req2.headers().size());
  UNIT_ASSERT_EQUAL("de.wikipedia.org", req2.host());
}

void RequestParserTest::test_common_get_request()
{
  request_parser parser;
  request req;
  size_t expected_header_size(4);
  UNIT_ASSERT_EQUAL(request_parser::FINISH, parser.parse(RequestData::GET_COMMON, req));
  UNIT_ASSERT_EQUAL(http::GET, req.method());
  UNIT_ASSERT_EQUAL("/api", req.url());
  UNIT_ASSERT_EQUAL(1, req.version().major);
  UNIT_ASSERT_EQUAL(1, req.version().minor);
  UNIT_ASSERT_EQUAL(expected_header_size, req.headers().size());
  UNIT_ASSERT_EQUAL("de.wikipedia.org", req.host());
  UNIT_ASSERT_EQUAL("application/x-www-form-urlencoded", req.content().type);
  UNIT_ASSERT_EQUAL("frog@jmarshall.com", req.headers().at(request_header::FROM));
  UNIT_ASSERT_EQUAL("HTTPTool/1.0", req.headers().at(request_header::USER_AGENT));
}

void RequestParserTest::test_common_get_query_request()
{
  request_parser parser;
  request req;
  size_t expected_header_size(4);
  UNIT_ASSERT_EQUAL(request_parser::FINISH, parser.parse(RequestData::GET_COMMON_QUERY, req));
  UNIT_ASSERT_EQUAL(http::GET, req.method());
  UNIT_ASSERT_EQUAL("/api", req.url());
  UNIT_ASSERT_EQUAL(1, req.version().major);
  UNIT_ASSERT_EQUAL(1, req.version().minor);
  UNIT_ASSERT_EQUAL(expected_header_size, req.headers().size());
  UNIT_ASSERT_EQUAL(2UL, req.query_params().size());
  UNIT_ASSERT_EQUAL("a==3&&c<=9", req.query_params().at("field"));
  UNIT_ASSERT_EQUAL("name", req.query_params().at("orderby"));
  UNIT_ASSERT_EQUAL("fragment", req.fragment());
  UNIT_ASSERT_EQUAL("de.wikipedia.org", req.host());
  UNIT_ASSERT_EQUAL("application/x-www-form-urlencoded", req.content().type);
  UNIT_ASSERT_EQUAL("frog@jmarshall.com", req.headers().at(request_header::FROM));
  UNIT_ASSERT_EQUAL("HTTPTool/1.0", req.headers().at(request_header::USER_AGENT));
}

void RequestParserTest::test_short_get_request()
{
  request_parser parser;
  request req;
  size_t expected_header_size(1);

  UNIT_ASSERT_EQUAL(request_parser::FINISH, parser.parse(RequestData::GET_SHORT, req));
  UNIT_ASSERT_EQUAL(http::GET, req.method());
  UNIT_ASSERT_EQUAL("/api", req.url());
  UNIT_ASSERT_EQUAL(1, req.version().major);
  UNIT_ASSERT_EQUAL(1, req.version().minor);
  UNIT_ASSERT_EQUAL(expected_header_size, req.headers().size());
  UNIT_ASSERT_EQUAL("de.wikipedia.org", req.host());
}

void RequestParserTest::test_query_post_request()
{
  request_parser parser;
  request req;
  size_t expected_header_size(3);
  size_t expected_content_length(32);

  UNIT_ASSERT_EQUAL(request_parser::FINISH, parser.parse(RequestData::POST_QUERY_BODY, req));
  UNIT_ASSERT_EQUAL(http::POST, req.method());
  UNIT_ASSERT_EQUAL("/enlighten/rest", req.url());
  UNIT_ASSERT_EQUAL(1, req.version().major);
  UNIT_ASSERT_EQUAL(1, req.version().minor);
  UNIT_ASSERT_EQUAL(expected_header_size, req.headers().size());
  UNIT_ASSERT_EQUAL("api.opencalais.com", req.host());
  UNIT_ASSERT_EQUAL("application/x-www-form-urlencoded", req.content().type);
  UNIT_ASSERT_EQUAL("32", req.content().length);
  UNIT_ASSERT_EQUAL(expected_content_length, req.body().size());
  UNIT_ASSERT_EQUAL("home=Cosby&favorite+flavor=flies", req.body());
}

void RequestParserTest::test_post_form_data_request()
{
  request_parser parser;
  request req;
  size_t expected_header_size {6};
  size_t expected_content_length {32};
  size_t expected_form_data_size {2};

  UNIT_ASSERT_EQUAL(request_parser::FINISH, parser.parse(RequestData::POST_FORM_DATA, req));
  UNIT_ASSERT_EQUAL(http::POST, req.method());
  UNIT_ASSERT_EQUAL("/experiments", req.url());
  UNIT_ASSERT_EQUAL(1, req.version().major);
  UNIT_ASSERT_EQUAL(1, req.version().minor);
  UNIT_ASSERT_EQUAL(expected_header_size, req.headers().size());
  UNIT_ASSERT_EQUAL("api.bonfire-project.eu:444", req.host());
  UNIT_ASSERT_EQUAL("*/*", req.headers().at(request_header::ACCEPT));
  UNIT_ASSERT_EQUAL("Basic XXX", req.headers().at(request_header::AUTHORIZATION));
  UNIT_ASSERT_EQUAL("gzip, deflate", req.headers().at(request_header::ACCEPT_ENCODING));
  UNIT_ASSERT_EQUAL(mime_types::APPLICATION_X_WWW_FORM_URLENCODED, req.content().type);
  UNIT_ASSERT_EQUAL("32", req.content().length);
  UNIT_ASSERT_EQUAL(expected_form_data_size, req.form_data().size());
  UNIT_ASSERT_EQUAL("carl", req.form_data().at("username"));
  UNIT_ASSERT_EQUAL("secret123", req.form_data().at("password"));
  UNIT_ASSERT_EQUAL(expected_content_length, req.body().size());
  UNIT_ASSERT_EQUAL("username=carl&password=secret123", req.body());

}

void RequestParserTest::test_xml_post_request()
{
  request_parser parser;
  request req;
  size_t expected_header_size(6);
  size_t expected_content_length(254);

  UNIT_ASSERT_EQUAL(request_parser::FINISH, parser.parse(RequestData::POST_XML_BODY, req));
  UNIT_ASSERT_EQUAL(http::POST, req.method());
  UNIT_ASSERT_EQUAL("/experiments", req.url());
  UNIT_ASSERT_EQUAL(1, req.version().major);
  UNIT_ASSERT_EQUAL(1, req.version().minor);
  UNIT_ASSERT_EQUAL(expected_header_size, req.headers().size());
  UNIT_ASSERT_EQUAL("api.bonfire-project.eu:444", req.host());
  UNIT_ASSERT_EQUAL("*/*", req.headers().at(request_header::ACCEPT));
  UNIT_ASSERT_EQUAL("Basic XXX", req.headers().at(request_header::AUTHORIZATION));
  UNIT_ASSERT_EQUAL("gzip, deflate", req.headers().at(request_header::ACCEPT_ENCODING));
  UNIT_ASSERT_EQUAL("application/vnd.bonfire+xml;charset=utf-8", req.content().type);
  UNIT_ASSERT_EQUAL("254", req.content().length);
  UNIT_ASSERT_EQUAL(expected_content_length, req.body().size());
//  UNIT_ASSERT_EQUAL("home=Cosby&favorite+flavor=flies", req.body);
}

void RequestParserTest::test_xml_post_partial_request()
{
  request_parser parser;
  request req;
  size_t expected_header_size_begin(1);
  size_t expected_header_size_end(6);
  size_t expected_content_length(254);

  /*
   * read in first part
   */
  UNIT_ASSERT_EQUAL(request_parser::PARTIAL, parser.parse(RequestData::POST_PARTIAL_BEGIN, req));
  UNIT_ASSERT_EQUAL(http::POST, req.method());
  UNIT_ASSERT_EQUAL("/experiments", req.url());
  UNIT_ASSERT_EQUAL(1, req.version().major);
  UNIT_ASSERT_EQUAL(1, req.version().minor);
  UNIT_ASSERT_EQUAL(expected_header_size_begin, req.headers().size());
  UNIT_ASSERT_EQUAL("api.bonfire-project.eu:444", req.host());

  /*
   * read in last part
   */
  UNIT_ASSERT_EQUAL(request_parser::FINISH, parser.parse(RequestData::POST_PARTIAL_END, req));
  UNIT_ASSERT_EQUAL(expected_header_size_end, req.headers().size());
  UNIT_ASSERT_EQUAL("*/*", req.headers().at(request_header::ACCEPT));
  UNIT_ASSERT_EQUAL("Basic XXX", req.headers().at(request_header::AUTHORIZATION));
  UNIT_ASSERT_EQUAL("gzip, deflate", req.headers().at(request_header::ACCEPT_ENCODING));
  UNIT_ASSERT_EQUAL("application/vnd.bonfire+xml; charset=utf-8", req.content().type);
  UNIT_ASSERT_EQUAL("254", req.content().length);
  UNIT_ASSERT_EQUAL(expected_content_length, req.body().size());
}

void RequestParserTest::test_xml_post_partial_request_2()
{
  request_parser parser;
  request req;
  size_t expected_header_size_begin(1);
  size_t expected_header_size_end(6);
  size_t expected_content_length(254);

  /*
   * read in first part
   */
  UNIT_ASSERT_EQUAL(request_parser::PARTIAL, parser.parse(RequestData::POST_PARTIAL_BEGIN_2, req));
  UNIT_ASSERT_EQUAL(http::POST, req.method());
  UNIT_ASSERT_EQUAL("/experiments", req.url());
  UNIT_ASSERT_EQUAL(1, req.version().major);
  UNIT_ASSERT_EQUAL(1, req.version().minor);
  UNIT_ASSERT_EQUAL(expected_header_size_begin, req.headers().size());
  UNIT_ASSERT_EQUAL("api.bonfire-project.eu:444", req.host());

  /*
   * read in middle part
   */
  UNIT_ASSERT_EQUAL(request_parser::PARTIAL, parser.parse(RequestData::POST_PARTIAL_MIDDLE_2, req));
  UNIT_ASSERT_EQUAL(expected_header_size_end, req.headers().size());
  UNIT_ASSERT_EQUAL("*/*", req.headers().at(request_header::ACCEPT));
  UNIT_ASSERT_EQUAL("Basic XXX", req.headers().at(request_header::AUTHORIZATION));
  UNIT_ASSERT_EQUAL("gzip, deflate", req.headers().at(request_header::ACCEPT_ENCODING));
  UNIT_ASSERT_EQUAL("application/vnd.bonfire+xml; charset=utf-8", req.content().type);
  UNIT_ASSERT_EQUAL("254", req.content().length);

  /*
   * read in last part
   */
  UNIT_ASSERT_EQUAL(request_parser::FINISH, parser.parse(RequestData::POST_PARTIAL_END_2, req));
  UNIT_ASSERT_EQUAL(expected_content_length, req.body().size());
}
