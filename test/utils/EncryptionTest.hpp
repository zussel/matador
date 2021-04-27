#ifndef MATADOR_ENCRYPTIONTEST_HPP
#define MATADOR_ENCRYPTIONTEST_HPP

#include "matador/unit/unit_test.hpp"

class EncryptionTest : public matador::unit_test
{
public:
  EncryptionTest();

  void test_sha256();
  void test_hmac();
};


#endif //MATADOR_ENCRYPTIONTEST_HPP
