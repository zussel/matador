#include "FileTestUnit.hpp"

#include "matador/utils/file.hpp"
#include "matador/utils/os.hpp"

using namespace matador;

FileTestUnit::FileTestUnit()
: unit_test("file", "file test unit")
{
  add_test("file", [this] { test_file(); }, "test file");
}

void FileTestUnit::test_file()
{
  file f("test.txt", "w");

  UNIT_ASSERT_TRUE(matador::os::exists("test.txt"));

  f.close();

  matador::os::remove("test.txt");

  UNIT_ASSERT_FALSE(matador::os::exists("test.txt"));
}
