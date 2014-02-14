#include "MemoryDatabaseTestUnit.hpp"

#include "connections.hpp"

using namespace oos;
using namespace std;

MemoryDatabaseTestUnit::MemoryDatabaseTestUnit()
  : DatabaseTestUnit("memory", "memory database test unit")
{}

MemoryDatabaseTestUnit::~MemoryDatabaseTestUnit()
{}
