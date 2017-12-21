#include "BlobTestUnit.hpp"

#include "matador/utils/blob.hpp"

#include <iostream>

using namespace matador;

BlobTestUnit::BlobTestUnit()
  : unit_test("blob", "blob test unit")
{
  add_test("create", std::bind(&BlobTestUnit::test_create, this), "create blob");
}

void BlobTestUnit::test_create()
{
  blob b1;

  int val = 8;
  b1.append(val);
  b1.append(val);

  UNIT_ASSERT_EQUAL(b1.size(), 8UL, "invalid size");

  blob b2;

  b2.append(val);
  b2.append(val);

  UNIT_ASSERT_EQUAL(b2.size(), 8UL, "invalid size");

  const unsigned char *data = b1.data();

  UNIT_ASSERT_TRUE(data != nullptr, "data must be valid pointer");
}
