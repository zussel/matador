#include "FileSystemTestUnit.hpp"

#include "fs/directory.hpp"

#include <iostream>
#include <sstream>
#include <string>

using namespace std;
using namespace oos;

FileSystemTestUnit::FileSystemTestUnit()
  : unit_test("fs", "file system test unit")
{
  add_test("simple", std::tr1::bind(&FileSystemTestUnit::simple_test, this), "simple fs test");
}

FileSystemTestUnit::~FileSystemTestUnit()
{
}

void FileSystemTestUnit::simple_test()
{
  directory dir;
  
  std::string path = dir.str();
  
  dir = oos::mkdir("test");
  
  UNIT_ASSERT_EQUAL(path + "test/", dir.str(), "directory was not created as expected");

  dir.remove(false);
  
  UNIT_ASSERT_EQUAL(path, dir.str(), "invalid directory");
  
  directory_iterator first = dir.directory_begin();
  directory_iterator last = dir.directory_end();
  while (first != last) {
    if (*first++ == (path + "test/")) {
      UNIT_FAIL("directory [" + path + "test/] was not removed");
    }
  }
  // creates a file in the current directory
  file test = file("test.txt");
  
  
}
