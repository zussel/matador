#include "JsonOrmTest.hpp"

JsonOrmTest::JsonOrmTest(const std::string &prefix, std::string dns)
  : unit_test(prefix + "_json_orm", prefix + " json orm test unit")
  , dns_(std::move(dns))
{
  add_test("insert", [this] { test_insert_from_json(); }, "insert from json string test");
}

void JsonOrmTest::test_insert_from_json()
{

}
