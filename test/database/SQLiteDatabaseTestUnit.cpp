#include "SQLiteDatabaseTestUnit.hpp"

#include "../Item.hpp"

#include "object/object_view.hpp"
#include "object/object_list.hpp"

#include "database/session.hpp"
#include "database/transaction.hpp"
#include "database/database_exception.hpp"

#include <iostream>
#include <fstream>
#include <cstdio>

using namespace oos;
using namespace std;

SQLiteDatabaseTestUnit::SQLiteDatabaseTestUnit()
  : DatabaseTestUnit("sqlite", "sqlite database test unit", "sqlite://test.sqlite")
{}

SQLiteDatabaseTestUnit::~SQLiteDatabaseTestUnit()
{}
