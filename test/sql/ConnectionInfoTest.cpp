#include "ConnectionInfoTest.hpp"

#include "matador/json/json_mapper.hpp"

#include "matador/sql/connection_info.hpp"

ConnectionInfoTest::ConnectionInfoTest()
: unit_test("connection_info", "connection info test")
{
  add_test("parse", [this] { test_parse_connection_info(); }, "test parse connection info");
  add_test("to_string", [this] { test_connection_info_to_string(); }, "test connection info to string");
  add_test("json", [this] { test_connection_info_from_json(); }, "test connection info from/to json");
}

void ConnectionInfoTest::test_parse_connection_info()
{
  const std::string mysql_dns = "mysql://test:test123@127.0.0.1/testdb";
  const std::string mssql_dns = "mssql://test:test123!@localhost/testdb (SQL Server)";
  const std::string postgresql_dns = "postgresql://test:test123@127.0.0.1:15432/test";
  const std::string sqlite_dns = "sqlite://test.sqlite";

  auto ci = matador::connection_info::parse(postgresql_dns, 15432);

  UNIT_ASSERT_EQUAL(ci.type, "postgresql");
  UNIT_ASSERT_EQUAL(ci.user, "test");
  UNIT_ASSERT_EQUAL(ci.password, "test123");
  UNIT_ASSERT_EQUAL(ci.hostname, "127.0.0.1");
  UNIT_ASSERT_EQUAL(ci.port, 15432);
  UNIT_ASSERT_EQUAL(ci.database, "test");
  UNIT_ASSERT_EQUAL(ci.driver, "");

  ci = matador::connection_info::parse(mysql_dns, 3306);

  UNIT_ASSERT_EQUAL(ci.type, "mysql");
  UNIT_ASSERT_EQUAL(ci.user, "test");
  UNIT_ASSERT_EQUAL(ci.password, "test123");
  UNIT_ASSERT_EQUAL(ci.hostname, "127.0.0.1");
  UNIT_ASSERT_EQUAL(ci.port, 3306);
  UNIT_ASSERT_EQUAL(ci.database, "testdb");
  UNIT_ASSERT_EQUAL(ci.driver, "");

  ci = matador::connection_info::parse(mssql_dns, 1433);

  UNIT_ASSERT_EQUAL(ci.type, "mssql");
  UNIT_ASSERT_EQUAL(ci.user, "test");
  UNIT_ASSERT_EQUAL(ci.password, "test123!");
  UNIT_ASSERT_EQUAL(ci.hostname, "localhost");
  UNIT_ASSERT_EQUAL(ci.port, 1433);
  UNIT_ASSERT_EQUAL(ci.database, "testdb");
  UNIT_ASSERT_EQUAL(ci.driver, "SQL Server");

  ci = matador::connection_info::parse(sqlite_dns, 0);

  UNIT_ASSERT_EQUAL(ci.type, "sqlite");
  UNIT_ASSERT_EQUAL(ci.user, "");
  UNIT_ASSERT_EQUAL(ci.password, "");
  UNIT_ASSERT_EQUAL(ci.hostname, "");
  UNIT_ASSERT_EQUAL(ci.port, 0);
  UNIT_ASSERT_EQUAL(ci.database, "test.sqlite");
  UNIT_ASSERT_EQUAL(ci.driver, "");
}

void ConnectionInfoTest::test_connection_info_to_string() {
  matador::connection_info ci;
  ci.type = "dbtype";
  ci.user = "test";
  ci.password = "test123%%";
  ci.hostname = "testhost";
  ci.port = 12345;
  ci.database = "db1";
  ci.driver = "SQL Client";

  auto dns = matador::connection_info::to_string(ci);

  UNIT_ASSERT_EQUAL(dns, "dbtype://test:test123%%@testhost:12345/db1 (SQL Client)");
}

void ConnectionInfoTest::test_connection_info_from_json()
{
  matador::connection_info ci;
  ci.type = "dbtype";
  ci.user = "test";
  ci.password = "test123%%";
  ci.hostname = "testhost";
  ci.port = 12345;
  ci.database = "db1";
  ci.driver = "(SQL Client)";

  matador::json_mapper mapper;

  auto expectedJsonString = R"foo({"type": "dbtype","user": "test","password": "test123%%","hostname": "testhost","port": 12345,"database": "db1","driver": "(SQL Client)"})foo";
  auto json = mapper.to_string(ci);

  UNIT_ASSERT_EQUAL(json, expectedJsonString);

  auto parsed_ci = mapper.to_object<matador::connection_info>(json);

  UNIT_ASSERT_EQUAL(parsed_ci.type, ci.type);
  UNIT_ASSERT_EQUAL(parsed_ci.user, ci.user);
  UNIT_ASSERT_EQUAL(parsed_ci.password, ci.password);
  UNIT_ASSERT_EQUAL(parsed_ci.hostname, ci.hostname);
  UNIT_ASSERT_EQUAL(parsed_ci.port, ci.port);
  UNIT_ASSERT_EQUAL(parsed_ci.database, ci.database);
  UNIT_ASSERT_EQUAL(parsed_ci.driver, ci.driver);
}
