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

#include "matador/unit/test_suite.hpp"

#include "unit/TestSuiteTestUnit.hpp"

//#include "utils/AnyTestUnit.hpp"
//#include "utils/BlobTestUnit.hpp"
//#include "utils/DateTestUnit.hpp"
//#include "utils/TimeTestUnit.hpp"
//#include "utils/VarCharTestUnit.hpp"
//#include "utils/FactoryTestUnit.hpp"
//#include "utils/StringTestUnit.hpp"
//#include "utils/SequencerTestUnit.hpp"

#include "object/ObjectStoreTestUnit.hpp"
//#include "object/ObjectPrototypeTestUnit.hpp"
//#include "object/ObjectTransactiontestUnit.hpp"
//#include "object/PrototypeTreeTest.hpp"
//#include "object/PrimaryKeyUnitTest.hpp"
//#include "object/HasManyVectorUnitTest.hpp"
//#include "object/HasManyListUnitTest.hpp"
#include "object/RelationTestUnit.hpp"
//
//#include "orm/OrmTestUnit.hpp"
//#include "orm/OrmReloadTestUnit.hpp"
#include "orm/OrmRelationTestUnit.hpp"
//#include "orm/TransactionTestUnit.hpp"
//
//#include "sql/ConnectionTestUnit.hpp"
//#include "sql/DialectTestUnit.hpp"
//#include "sql/QueryTestUnit.hpp"
//#include "sql/ConditionUnitTest.hpp"
//#include "sql/MSSQLDialectTestUnit.hpp"
//#include "sql/SQLiteDialectTestUnit.hpp"

#include "connections.hpp"

#ifdef OOS_MYSQL
#ifdef WIN32
#include <mysql_version.h>
#else
#include <mysql/mysql_version.h>
#endif
#endif

#include <cstdlib> // EXIT_SUCCESS

using namespace matador;

int main(int argc, char *argv[])
{
  matador::test_suite suite;

//  TestSuiteTestUnit test_suite_test;
//  bool result = test_suite_test.test_create();
//  result &= test_suite_test.test_list();
//  result &= test_suite_test.test_init();
//  result &= test_suite_test.test_method();
//  result &= test_suite_test.test_unit();
//  result &= test_suite_test.test_suite();
//
//  if (!result) {
//    return EXIT_FAILURE;
//  }

  suite.init(argc, argv);

//  suite.register_unit(new AnyTestUnit);
//  suite.register_unit(new DateTestUnit);
//  suite.register_unit(new TimeTestUnit);
//  suite.register_unit(new BlobTestUnit);
//  suite.register_unit(new VarCharTestUnit);
//  suite.register_unit(new FactoryTestUnit);
//  suite.register_unit(new StringTestUnit);
//  suite.register_unit(new SequencerTestUnit);
//
//  suite.register_unit(new PrimaryKeyUnitTest);
//  suite.register_unit(new PrototypeTreeTestUnit);
//  suite.register_unit(new ObjectPrototypeTestUnit);
  suite.register_unit(new ObjectStoreTestUnit);
//  suite.register_unit(new ObjectTransactiontestUnit);
//  suite.register_unit(new HasManyVectorUnitTest);
//  suite.register_unit(new HasManyListUnitTest);
  suite.register_unit(new RelationTestUnit);
//
//  suite.register_unit(new ConditionUnitTest);
//  suite.register_unit(new DialectTestUnit);

//#if defined(MATADOR_MYSQL) && defined(MATADOR_MYSQL_TEST)
//  suite.register_unit(new ConnectionTestUnit("mysql_conn", "mysql connection test unit", ::connection::mysql));
//  suite.register_unit(new TransactionTestUnit("mysql_transaction", "mysql transaction test unit", ::connection::mysql));
//  suite.register_unit(new QueryTestUnit("mysql_query", "mysql query test unit", ::connection::mysql, matador::time(2015, 3, 15, 13, 56, 23)));
//  suite.register_unit(new OrmTestUnit("mysql", ::connection::mysql));
//  suite.register_unit(new OrmReloadTestUnit("mysql", ::connection::mysql));
//  suite.register_unit(new OrmRelationTestUnit("mysql", ::connection::mysql));
//#endif
//
//#if defined(MATADOR_ODBC) && defined(MATADOR_ODBC_TEST)
//  suite.register_unit(new ConnectionTestUnit("mssql_conn", "mssql connection test unit", ::connection::mssql));
//  suite.register_unit(new TransactionTestUnit("mssql_transaction", "mssql transaction test unit", ::connection::mssql));
//  suite.register_unit(new QueryTestUnit("mssql_query", "mssql query test unit", ::connection::mssql));
//  suite.register_unit(new OrmTestUnit("mssql", ::connection::mssql));
//  suite.register_unit(new OrmReloadTestUnit("mssql", ::connection::mssql));
//  suite.register_unit(new OrmRelationTestUnit("mssql", ::connection::mssql));
//  suite.register_unit(new MSSQLDialectTestUnit());
//#endif
//
#if defined(MATADOR_SQLITE3) && defined(MATADOR_SQLITE3_TEST)
//  suite.register_unit(new ConnectionTestUnit("sqlite_conn", "sqlite connection test unit", ::connection::sqlite));
//  suite.register_unit(new TransactionTestUnit("sqlite_transaction", "sqlite transaction test unit", ::connection::sqlite));
//  suite.register_unit(new QueryTestUnit("sqlite_query", "sqlite query test unit", ::connection::sqlite));
//  suite.register_unit(new OrmTestUnit("sqlite", ::connection::sqlite));
//  suite.register_unit(new OrmReloadTestUnit("sqlite", ::connection::sqlite));
  suite.register_unit(new OrmRelationTestUnit("sqlite", ::connection::sqlite));
//  suite.register_unit(new SQLiteDialectTestUnit());
#endif

//  suite.register_unit(new TransactionTestUnit("memory_transaction", "memory transaction test unit"));

  bool result = suite.run();
  return result ? EXIT_SUCCESS : EXIT_FAILURE;
}
