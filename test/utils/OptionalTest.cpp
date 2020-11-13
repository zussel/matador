#include "OptionalTest.hpp"

#include "matador/utils/optional.hpp"

using namespace matador;

OptionalTest::OptionalTest()
  : unit_test("optional", "optional test")
{
  add_test("bad_access", [this] { test_bad_access(); }, "optional bad access test");
}

void OptionalTest::test_bad_access()
{
  optional<int> bad_int;

  UNIT_ASSERT_EXCEPTION(bad_int.value(), bad_optional_access, "bad optional access");
}
