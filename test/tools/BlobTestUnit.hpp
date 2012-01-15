#ifndef BLOBTESTUNIT_HPP
#define BLOBTESTUNIT_HPP

#include "unit/unit_test.hpp"

class BlobTestUnit : public oos::unit_test
{
public:
  BlobTestUnit();
  virtual ~BlobTestUnit();
  
  void create_blob();

  /**
   * Initializes a test unit
   */
  virtual void initialize() {}
  virtual void finalize() {}
};

#endif /* BLOBTESTUNIT_HPP */

