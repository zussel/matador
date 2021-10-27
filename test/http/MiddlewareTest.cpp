#include "MiddlewareTest.hpp"

#include "matador/http/middleware.hpp"
#include "matador/http/request.hpp"
#include "matador/http/mime_types.hpp"

MiddlewareTest::MiddlewareTest()
  : matador::unit_test("middleware", "middleware test")
{
  add_test("middleware", [this]() { test_middleware(); }, "middleware test");
}

using namespace matador::http;

class testing_middleware : public middleware
{
public:
  matador::http::response process(request &, const next_func_t &next) override
  {
    auto resp = next();
    return response::ok("check", mime_types::TYPE_TEXT_PLAIN);
  }
};

void MiddlewareTest::test_middleware()
{
  middleware_pipeline mp;

  auto tmw = std::make_shared<testing_middleware>();

  mp.add(tmw);

  request req;

  auto resp = mp.process(req);

  UNIT_ASSERT_EQUAL(http::OK, resp.status());
  UNIT_ASSERT_EQUAL("check", resp.body());
  UNIT_ASSERT_EQUAL(mime_types::TEXT_PLAIN, resp.content().type);
}
