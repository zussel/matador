#include "BlobTestUnit.hpp"

#include "tools/blob.hpp"

#include <iostream>

using namespace oos;

using std::cout;

BlobTestUnit::BlobTestUnit()
  : unit_test("blob test unit")
{
  add_test("create", std::tr1::bind(&BlobTestUnit::create_blob, this), "create blob");
}

BlobTestUnit::~BlobTestUnit()
{}

void BlobTestUnit::create_blob()
{
  cout << "\n";

  blob<32> b;
  
  int val = 8;
  b.assign(val);
  b.assign(val);

  cout << "size: " << b.size() << "\n";
  cout << "max size: " << b.max_size() << "\n";

  blob<0> b2;
  
  b2.assign(val);
  b2.assign(val);

  cout << "size: " << b2.size() << "\n";
  cout << "max size: " << b2.max_size() << "\n";
}
