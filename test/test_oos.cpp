/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#include "unit/FirstTestUnit.hpp"
#include "unit/SecondTestUnit.hpp"

#include "tools/ConvertTestUnit.hpp"
#include "tools/BlobTestUnit.hpp"
#include "tools/VarCharTestUnit.hpp"
#include "tools/FactoryTestUnit.hpp"

#include "object/ObjectStoreTestUnit.hpp"
#include "object/ObjectPrototypeTestUnit.hpp"
#include "object/ObjectListTestUnit.hpp"
#include "object/ObjectVectorTestUnit.hpp"
#include "object/PrototypeTreeTest.hpp"

#include "database/DatabaseTestUnit.hpp"
#include "database/SessionTestUnit.hpp"
#include "database/TransactionTestUnit.hpp"

#include "json/JsonTestUnit.hpp"

#include "connections.hpp"

using namespace oos;

int main(int argc, char *argv[])
{
  test_suite::instance().init(argc, argv);

  test_suite::instance().register_unit(new FirstTestUnit());
  test_suite::instance().register_unit(new SecondTestUnit());

  test_suite::instance().register_unit(new ConvertTestUnit());
  test_suite::instance().register_unit(new BlobTestUnit());
  test_suite::instance().register_unit(new VarCharTestUnit());
  test_suite::instance().register_unit(new FactoryTestUnit());

  test_suite::instance().register_unit(new PrototypeTreeTestUnit());
  test_suite::instance().register_unit(new ObjectPrototypeTestUnit());
  test_suite::instance().register_unit(new ObjectStoreTestUnit());
  test_suite::instance().register_unit(new ObjectListTestUnit());
  test_suite::instance().register_unit(new ObjectVectorTestUnit());

#ifdef OOS_MYSQL
  test_suite::instance().register_unit(new SessionTestUnit("mysql", "mysql session test unit", connection::mysql));
  test_suite::instance().register_unit(new TransactionTestUnit("mysql", "mysql transaction test unit", connection::mysql));
  test_suite::instance().register_unit(new DatabaseTestUnit("mysql", "mysql database test unit", connection::mysql));
#endif

#ifdef OOS_ODBC
  test_suite::instance().register_unit(new SessionTestUnit("mssql", "mssql session test unit", connection::mssql));
  test_suite::instance().register_unit(new TransactionTestUnit("mssql", "mssql transaction test unit", connection::mssql));
  test_suite::instance().register_unit(new DatabaseTestUnit("mssql", "mssql database test unit", connection::mssql));
#endif

#ifdef OOS_SQLITE3
  test_suite::instance().register_unit(new SessionTestUnit("sqlite", "sqlite session test unit", connection::sqlite));
  test_suite::instance().register_unit(new TransactionTestUnit("sqlite", "sqlite transaction test unit", connection::sqlite));
  test_suite::instance().register_unit(new DatabaseTestUnit("sqlite", "sqlite database test unit", connection::sqlite));
#endif

  test_suite::instance().register_unit(new TransactionTestUnit("memory", "memory transaction test unit"));

  test_suite::instance().register_unit(new JsonTestUnit());

  return test_suite::instance().run() ? 0 : 1;
}
