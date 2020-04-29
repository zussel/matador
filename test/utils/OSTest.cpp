#include "OSTest.hpp"

#include "matador/utils/os.hpp"

using namespace matador;

OSTest::OSTest()
  : unit_test("os", "os test")
{
  add_test("mkdir", std::bind(&OSTest::test_mkdir, this), "os mkdir test");
}

void OSTest::test_mkdir()
{
  std::string path("temppath");
  // check if dir exists
  UNIT_ASSERT_FALSE(os::exists(path));

  // create dir
  UNIT_ASSERT_TRUE(os::mkdir(path));

  // test if dir exists
  UNIT_ASSERT_TRUE(os::exists(path));

  // delete dir
  UNIT_ASSERT_TRUE(os::rmdir(path));
}
