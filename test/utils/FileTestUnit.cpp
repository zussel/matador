#include "FileTestUnit.hpp"

#include "matador/utils/file.hpp"
#include "matador/utils/os.hpp"

using namespace matador;

FileTestUnit::FileTestUnit()
: unit_test("file", "file test unit")
{
  add_test("file", std::bind(&FileTestUnit::test_file, this), "test file");
}

void FileTestUnit::test_file()
{
  file f("test.txt", "w");

  UNIT_ASSERT_TRUE(matador::os::exists("test.txt"));

  matador::os::remove("test.txt");

  UNIT_ASSERT_FALSE(matador::os::exists("test.txt"));
}
