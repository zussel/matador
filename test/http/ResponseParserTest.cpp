#include "ResponseParserTest.hpp"
#include "ResponseData.hpp"

#include "matador/http/response_parser.hpp"
#include "matador/http/response.hpp"

#include "matador/utils/json_mapper.hpp"
#include "matador/utils/file.hpp"
#include "matador/utils/os.hpp"

using namespace matador;

ResponseParserTest::ResponseParserTest()
  : matador::unit_test("response_parser", "response parser test")
{
  add_test("empty_body", [this]() { test_empty_response(); }, "test empty body response");
  add_test("body", [this]() { test_body_response(); }, "test filled body response");
  add_test("from_file", [this]() { test_from_file_response(); }, "test from file response");
}

void ResponseParserTest::test_empty_response()
{
  http::response_parser parser;

  http::response resp;

  auto result = parser.parse(ResponseData::EMPTY_BODY, resp);

  UNIT_ASSERT_TRUE(result);
  UNIT_ASSERT_EQUAL(http::http::NO_CONTENT, resp.status());
  UNIT_ASSERT_EQUAL(1, resp.version().major);
  UNIT_ASSERT_EQUAL(1, resp.version().minor);
  UNIT_ASSERT_EQUAL(1UL, resp.headers().size());
}

namespace response_detail {
struct person
{
  std::string name;
  int age {};

  template < class S >
  void serialize(S &serializer)
  {
    serializer.serialize("name", name);
    serializer.serialize("age", age);
  }
};
}

void ResponseParserTest::test_body_response()
{
  http::response_parser parser;

  http::response resp;

  auto result = parser.parse(ResponseData::FILLED_BODY, resp);

  UNIT_ASSERT_TRUE(result);
  UNIT_ASSERT_EQUAL(http::http::OK, resp.status());
  UNIT_ASSERT_EQUAL(3UL, resp.headers().size());
  UNIT_ASSERT_EQUAL(1, resp.version().major);
  UNIT_ASSERT_EQUAL(1, resp.version().minor);

  json_mapper<response_detail::person> mapper;
  auto obj = mapper.object_from_string(resp.body().c_str());

  UNIT_ASSERT_EQUAL("George", obj.name);
  UNIT_ASSERT_EQUAL(37, obj.age);
}

void ResponseParserTest::test_from_file_response()
{
  const std::string filename { "dummy.html" };

  file f(filename, "w");

  UNIT_ASSERT_TRUE(matador::os::exists(filename));

  const std::string data { "<body><h1>hello world</h1></body>"};
  ::fwrite(data.c_str(), sizeof(char), data.size(), f.stream());
  f.close();

  http::response resp = http::response::from_file("/" + filename);

  UNIT_ASSERT_EQUAL(http::http::OK, resp.status());
  UNIT_ASSERT_EQUAL(3UL, resp.headers().size());
  UNIT_ASSERT_EQUAL(1, resp.version().major);
  UNIT_ASSERT_EQUAL(1, resp.version().minor);
  UNIT_ASSERT_EQUAL(data, resp.body());
  UNIT_ASSERT_EQUAL(http::mime_types::TEXT_HTML, resp.content().type);

  matador::os::remove(filename);

  UNIT_ASSERT_FALSE(matador::os::exists(filename));
}