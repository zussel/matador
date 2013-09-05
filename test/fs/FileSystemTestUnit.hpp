#ifndef FILESYSTEMTESTUNIT_HPP
#define FILESYSTEMTESTUNIT_HPP

#include "unit/unit_test.hpp"

class FileSystemTestUnit : public oos::unit_test
{
public:
  FileSystemTestUnit();
  ~FileSystemTestUnit();
  
  void simple_test();

  /**
   * Initializes a test unit
   */
  virtual void initialize() {}
  virtual void finalize() {}  
};

#endif /* FILESYSTEMTESTUNIT_HPP */
