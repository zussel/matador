#include "EncryptionTest.hpp"

#include "matador/utils/sha256.hpp"
#include "matador/utils/hmac.hpp"

EncryptionTest::EncryptionTest()
  : matador::unit_test("encryption", "sha256 test")
{
  add_test("sha256", [this] { test_sha256(); }, "test sha256 hashing");
  add_test("hmac", [this] { test_hmac(); }, "test hmac encryption");
}

using namespace matador::ext;

void EncryptionTest::test_sha256()
{
  auto enc = sha256("hello world");

  UNIT_ASSERT_EQUAL("b94d27b9934d3e08a52e52d7da7dabfac484efe37a5380ee9088f7ace2efcde9", enc);

  const char *test { "foo bar" };

  enc = sha256(test, strlen(test));

  UNIT_ASSERT_EQUAL("fbc1a9f858ea9e177916964bd88c3d37b91a1e84412765e29950777f265c4b75", enc);
}

void EncryptionTest::test_hmac()
{
  const std::string key { "mykey" };
  const std::string message { "helloworld" };

  auto result = matador::hmac(key, message);

  UNIT_ASSERT_EQUAL("7fdfaa9c9c0931f52d9ebf2538bc99700f2e771f3af1c1d93945c2256c11aedd", result);
}
