#include <catch2/catch_test_macros.hpp>

#include <matador/sql/table_definition.hpp>

#include <list>

using namespace matador::sql;

TEST_CASE("Create record", "[record]") {
  table_definition def({
           make_pk_column<unsigned long>("id"),
           make_column<std::string>("name", 255),
           make_column<std::string>("color", 255)
  });

  REQUIRE(def.size() == 3);

  std::list<std::string> expected_columns = {"id", "name", "color"};
  for(const auto &col : expected_columns) {
    REQUIRE(def.find(col) != def.end());
  }

  for(const auto& col : def) {
    expected_columns.remove(col.name());
  }

  REQUIRE(expected_columns.empty());
}

TEST_CASE("Append to record", "[record]") {
  table_definition rec;

  rec.append(make_pk_column<unsigned long>("id"));
  rec.append<std::string>("name", 255);
  rec.append<std::string>("color", 63);

  REQUIRE(rec.size() == 3);

  std::list<std::string> expected_columns = {"id", "name", "color"};
  for(const auto &col : expected_columns) {
    REQUIRE(rec.find(col) != rec.end());
  }

  for(const auto& col : rec) {
    expected_columns.remove(col.name());
  }

  REQUIRE(expected_columns.empty());
}