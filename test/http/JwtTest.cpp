#include "JwtTest.hpp"

#include "matador/utils/json_mapper.hpp"
#include "matador/utils/json_serializer.hpp"

#include "matador/http/auth/jwt_token.hpp"

JwtTest::JwtTest()
  : matador::unit_test("jwt", "jwt test")
{
  add_test("token", [this]() { test_jwt_token(); }, "jwt token test");
}

using namespace matador::http::auth;
using namespace matador;

void JwtTest::test_jwt_token()
{
  jwt_header h { "HSA256", "JWT" };

  json_mapper mapper;

  auto result = mapper.to_string(h);

  UNIT_ASSERT_EQUAL(R"({"alg": "HSA256","typ": "JWT"})", result);

  jwt_payload p;
  p.issuer = "www.example.com";
  p.subject = "123456789";
  p.name = "John Doe";

  result = mapper.to_string(p);

  UNIT_ASSERT_EQUAL(R"({"iss": "www.example.com","sub": "123456789","name": "John Doe"})", result);
}
