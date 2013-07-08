#include "BlobTestUnit.hpp"

#include "tools/blob.hpp"

#include <iostream>

using namespace oos;

using std::cout;

BlobTestUnit::BlobTestUnit()
  : unit_test("blob", "blob test unit")
{
  add_test("create", std::tr1::bind(&BlobTestUnit::create_blob, this), "create blob");
}

BlobTestUnit::~BlobTestUnit()
{}

void BlobTestUnit::create_blob()
{
  /*
  cout << "\n";

  blob b1;
  
  int val = 8;
  b1.assign(val);
  b1.assign(val);

  cout << "size: " << b1.size() << "\n";
  cout << "capacity: " << b1.capacity() << "\n";

  blob b2;
  
  b2.assign(val);
  b2.assign(val);

  cout << "size: " << b2.size() << "\n";
  cout << "capacity: " << b2.capacity() << "\n";
  */
}
