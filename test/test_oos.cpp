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
#include "database/MySQLBaseTestUnit.hpp"

#include "json/JsonTestUnit.hpp"

#include "unit/test_suite.hpp"

using namespace oos;

int main(int argc, char *argv[])
{
  test_suite::instance().init(argc, argv);

  test_suite::instance().register_unit("first", new FirstTestUnit());
  test_suite::instance().register_unit("second", new SecondTestUnit());

  test_suite::instance().register_unit("convert", new ConvertTestUnit());
  test_suite::instance().register_unit("blob", new BlobTestUnit());
  test_suite::instance().register_unit("varchar", new VarCharTestUnit());
  test_suite::instance().register_unit("factory", new FactoryTestUnit());

  test_suite::instance().register_unit("prototype", new ObjectPrototypeTestUnit());
  test_suite::instance().register_unit("store", new ObjectStoreTestUnit());
  test_suite::instance().register_unit("list", new ObjectListTestUnit());
  test_suite::instance().register_unit("vector", new ObjectVectorTestUnit());
  test_suite::instance().register_unit("mysql_base", new MySQLBaseTestUnit());
  test_suite::instance().register_unit("mysql", new MySQLDatabaseTestUnit());
  test_suite::instance().register_unit("sqlite", new SQLiteDatabaseTestUnit());

  test_suite::instance().register_unit("json", new JsonTestUnit());

  test_suite::instance().run();
  
  return 0;
}
