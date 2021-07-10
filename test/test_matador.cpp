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

#include "matador/net/os.hpp"

#include "unit/TestSuiteTestUnit.hpp"

#include "logger/LoggerTest.hpp"

#include "matador/utils/os.hpp"

#include "utils/TimeTestUnit.hpp"

#include "utils/AnyTestUnit.hpp"
#include "utils/Base64Test.hpp"
#include "utils/BufferViewTest.hpp"
#include "utils/BlobTestUnit.hpp"
#include "utils/DateTestUnit.hpp"
#include "utils/FileTestUnit.hpp"
#include "utils/JsonTestUnit.hpp"
#include "utils/JsonMapperTestUnit.hpp"
#include "utils/JsonSerializerTest.hpp"
#include "utils/FactoryTestUnit.hpp"
#include "utils/StringTestUnit.hpp"
#include "utils/SequencerTestUnit.hpp"
#include "utils/OSTest.hpp"
#include "utils/TreeTest.hpp"
#include "utils/ThreadPoolTest.hpp"
#include "utils/StreamsTest.hpp"
#include "utils/OptionalTest.hpp"
#include "utils/EncryptionTest.hpp"
#include "utils/HtmlTest.hpp"
#include "utils/UrlTest.hpp"

#include "object/ObjectStoreTestUnit.hpp"
#include "object/ObjectPrototypeTestUnit.hpp"
#include "object/ObjectTransactionTestUnit.hpp"
#include "object/PrototypeTreeTest.hpp"
#include "object/PrimaryKeyUnitTest.hpp"
#include "object/JsonObjectMapperTest.hpp"
#include "object/HasManyUnitTest.hpp"
#include "object/HasManyVectorUnitTest.hpp"
#include "object/HasManyListUnitTest.hpp"
#include "object/RelationTestUnit.hpp"

#include "orm/BlogUnitTest.hpp"
#include "orm/PrimaryKeyTestUnit.hpp"
#include "orm/OrmTestUnit.hpp"
#include "orm/OrmReloadTestUnit.hpp"
#include "orm/OrmRelationTestUnit.hpp"
#include "orm/TransactionTestUnit.hpp"

#include "sql/ConnectionTestUnit.hpp"
#include "sql/DialectTestUnit.hpp"
#include "sql/QueryTestUnit.hpp"
#include "sql/ConditionUnitTest.hpp"
#include "sql/MSSQLDialectTestUnit.hpp"
#include "sql/PostgreSQLDialectTestUnit.hpp"
#include "sql/SQLiteDialectTestUnit.hpp"
#include "sql/ValueUnitTest.hpp"
#include "sql/SqlLoggerTest.hpp"

#include "net/IPTestUnit.hpp"
#include "net/AddressTest.hpp"
#include "net/SocketTest.hpp"
#include "net/FDSetTest.hpp"
#include "net/ReactorTest.hpp"
#include "net/IOServiceTest.hpp"
#include "net/AddressResolverTest.hpp"
#include "net/SocketInterrupterTest.hpp"

#include "http/HttpServerTest.hpp"
#include "http/JwtTest.hpp"
#include "http/RequestParserTest.hpp"
#include "http/ResponseParserTest.hpp"
#include "http/RouteEngineTest.hpp"
#include "http/RouteEndpointTest.hpp"
#include "http/TemplateEngineTest.hpp"
#include "http/MiddlewareTest.hpp"

#include "connections.hpp"

#include <cstdlib> // EXIT_SUCCESS

using namespace matador;

int main(int argc, char *argv[])
{
#ifdef _WIN32
  _putenv_s("TZ", "/usr/share/zoneinfo/Europe/Berlin");
  _tzset();
#else
  setenv("TZ", "/usr/share/zoneinfo/Europe/Berlin", 1);
  tzset();
#endif

  matador::test_suite suite;

  TestSuiteTestUnit test_suite_test;
  bool result = test_suite_test.test_create();
  result &= test_suite_test.test_list();
  result &= test_suite_test.test_init();
  result &= test_suite_test.test_method();
  result &= test_suite_test.test_unit();
  result &= test_suite_test.test_suite();

  if (!result) {
    return EXIT_FAILURE;
  }

  suite.init(argc, argv);

  suite.register_unit(new AnyTestUnit);
  suite.register_unit(new Base64Test);
  suite.register_unit(new BufferViewTest);
  suite.register_unit(new DateTestUnit);
  suite.register_unit(new TimeTestUnit);
  suite.register_unit(new FileTestUnit);
  suite.register_unit(new BlobTestUnit);
  suite.register_unit(new JsonTestUnit);
  suite.register_unit(new JsonMapperTestUnit);
  suite.register_unit(new JsonSerializerTest);
  suite.register_unit(new FactoryTestUnit);
  suite.register_unit(new StringTestUnit);
  suite.register_unit(new SequencerTestUnit);
  suite.register_unit(new OSTest);
  suite.register_unit(new TreeTest);
  suite.register_unit(new ThreadPoolTest);
  suite.register_unit(new StreamsTest);
  suite.register_unit(new OptionalTest);
  suite.register_unit(new EncryptionTest);
  suite.register_unit(new HtmlTest);
  suite.register_unit(new UrlTest);

  suite.register_unit(new LoggerTest);

  suite.register_unit(new PrimaryKeyUnitTest);
  suite.register_unit(new PrototypeTreeTestUnit);
  suite.register_unit(new ObjectPrototypeTestUnit);
  suite.register_unit(new ObjectStoreTestUnit);
  suite.register_unit(new ObjectTransactionTestUnit);
  suite.register_unit(new HasManyUnitTest);
  suite.register_unit(new JsonObjectMapperTest);
  suite.register_unit(new HasManyVectorUnitTest);
  suite.register_unit(new HasManyListUnitTest);
  suite.register_unit(new RelationTestUnit);

  suite.register_unit(new ConditionUnitTest);
  suite.register_unit(new DialectTestUnit);
  suite.register_unit(new SqlLoggerTest);
  suite.register_unit(new ValueUnitTest);

  suite.register_unit(new IPTestUnit);
  suite.register_unit(new AddressTest);
  suite.register_unit(new SocketTest);
  suite.register_unit(new FDSetTest);
  suite.register_unit(new ReactorTest);
  suite.register_unit(new IOServiceTest);
  suite.register_unit(new AddressResolverTest);
  suite.register_unit(new SocketInterrupterTest);

  suite.register_unit(new HttpServerTest);
  suite.register_unit(new JwtTest);
  suite.register_unit(new RequestParserTest);
  suite.register_unit(new ResponseParserTest);
  suite.register_unit(new RouteEngineTest);
  suite.register_unit(new RouteEndpointTest);
  suite.register_unit(new TemplateEngineTest);
  suite.register_unit(new MiddlewareTest);

#if defined(MATADOR_MYSQL) && defined(MATADOR_MYSQL_TEST)
  suite.register_unit(new ConnectionTestUnit("mysql", ::connection::mysql));
  suite.register_unit(new TransactionTestUnit("mysql", ::connection::mysql));
  suite.register_unit(new QueryTestUnit("mysql", ::connection::mysql, matador::time(2015, 3, 15, 13, 56, 23)));
  suite.register_unit(new BlogUnitTest("mysql", ::connection::mysql));
  suite.register_unit(new OrmTestUnit("mysql", ::connection::mysql));
  suite.register_unit(new OrmReloadTestUnit("mysql", ::connection::mysql));
  suite.register_unit(new OrmRelationTestUnit("mysql", ::connection::mysql));
#endif

#if defined(MATADOR_ODBC) && defined(MATADOR_ODBC_TEST)
  suite.register_unit(new ConnectionTestUnit("mssql", ::connection::mssql));
  suite.register_unit(new TransactionTestUnit("mssql", ::connection::mssql));
  suite.register_unit(new QueryTestUnit("mssql", ::connection::mssql));
  suite.register_unit(new BlogUnitTest("mssql", ::connection::mssql));
  suite.register_unit(new OrmTestUnit("mssql", ::connection::mssql));
  suite.register_unit(new OrmReloadTestUnit("mssql", ::connection::mssql));
  suite.register_unit(new OrmRelationTestUnit("mssql", ::connection::mssql));
  suite.register_unit(new MSSQLDialectTestUnit());
#endif

#if defined(MATADOR_SQLITE3) && defined(MATADOR_SQLITE3_TEST)
  suite.register_unit(new ConnectionTestUnit("sqlite", ::connection::sqlite));
  suite.register_unit(new TransactionTestUnit("sqlite", ::connection::sqlite));
  suite.register_unit(new QueryTestUnit("sqlite", ::connection::sqlite));
  suite.register_unit(new BlogUnitTest("sqlite", ::connection::sqlite));
  suite.register_unit(new PrimaryKeyTestUnit("sqlite", ::connection::sqlite));
  suite.register_unit(new OrmTestUnit("sqlite", ::connection::sqlite));
  suite.register_unit(new OrmReloadTestUnit("sqlite", ::connection::sqlite));
  suite.register_unit(new OrmRelationTestUnit("sqlite", ::connection::sqlite));
  suite.register_unit(new SQLiteDialectTestUnit());
#endif

#if defined(MATADOR_POSTGRESQL) && defined(MATADOR_POSTGRESQL_TEST)
  suite.register_unit(new ConnectionTestUnit("postgresql", ::connection::postgresql));
  suite.register_unit(new TransactionTestUnit("postgresql", ::connection::postgresql));
  suite.register_unit(new QueryTestUnit("postgresql", ::connection::postgresql));
  suite.register_unit(new OrmTestUnit("postgresql", ::connection::postgresql));
  suite.register_unit(new OrmReloadTestUnit("postgresql", ::connection::postgresql));
  suite.register_unit(new OrmRelationTestUnit("postgresql", ::connection::postgresql));
  suite.register_unit(new PostgreSQLDialectTestUnit());
#endif

//  suite.register_unit(new TransactionTestUnit("memory_transaction", "memory transaction test unit"));

  net::init();

  result = suite.run();

  net::cleanup();
  return result ? EXIT_SUCCESS : EXIT_FAILURE;
}
