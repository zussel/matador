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


#include "database/SQLiteDatabaseTestUnit.hpp"
#include "database/MySQLDatabaseTestUnit.hpp"
#include "database/MSSQLDatabaseTestUnit.hpp"
#include "json/JsonTestUnit.hpp"

#include "unit/test_suite.hpp"

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

  test_suite::instance().register_unit(new ObjectPrototypeTestUnit());
  test_suite::instance().register_unit(new ObjectStoreTestUnit());
  test_suite::instance().register_unit(new ObjectListTestUnit());
  test_suite::instance().register_unit(new ObjectVectorTestUnit());
#if MYSQL_FOUND
  test_suite::instance().register_unit(new MySQLDatabaseTestUnit());
#endif
#if ODBC_FOUND
  test_suite::instance().register_unit(new MSSQLDatabaseTestUnit());
#endif
  test_suite::instance().register_unit(new SQLiteDatabaseTestUnit());

  test_suite::instance().register_unit(new JsonTestUnit());

  test_suite::instance().run();
  
  return 0;
}
