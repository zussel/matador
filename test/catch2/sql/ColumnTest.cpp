#include <catch2/catch_test_macros.hpp>

#include "matador/sql/column_definition.hpp"

using namespace matador::sql;

TEST_CASE("Create empty column", "[column]") {
  column_definition c("name");

  REQUIRE(c.name() == "name");
  REQUIRE(c.index() == -1);
  REQUIRE(c.type() == matador::data_type::type_unknown);
  REQUIRE(c.ref_table().empty());
  REQUIRE(c.ref_column().empty());

  c.set(std::string{"george"}, 255);
  REQUIRE(c.type() == matador::data_type::type_varchar);
  REQUIRE(c.as<std::string>() == "george");

  c.set(7);
  REQUIRE(c.type() == matador::data_type::type_int);
  REQUIRE(c.as<std::string>() == "7");
  REQUIRE(c.as<long>() == 7);
  REQUIRE(c.str() == "7");
}

TEST_CASE("Copy and move column", "[column]") {
  column_definition c("name");
  c.set(std::string{"george"}, 255);
  REQUIRE(c.name() == "name");
  REQUIRE(c.index() == -1);
  REQUIRE(c.ref_table().empty());
  REQUIRE(c.ref_column().empty());
  REQUIRE(c.type() == matador::data_type::type_varchar);
  REQUIRE(c.as<std::string>() == "george");
  REQUIRE(c.attributes().size() == 255);

  auto c2 = c;
  REQUIRE(c2.name() == "name");
  REQUIRE(c2.index() == -1);
  REQUIRE(c2.ref_table().empty());
  REQUIRE(c2.ref_column().empty());
  REQUIRE(c2.type() == matador::data_type::type_varchar);
  REQUIRE(c2.as<std::string>() == "george");
  REQUIRE(c2.attributes().size() == 255);

  auto c3 = std::move(c2);
  REQUIRE(c3.name() == "name");
  REQUIRE(c3.index() == -1);
  REQUIRE(c3.ref_table().empty());
  REQUIRE(c3.ref_column().empty());
  REQUIRE(c3.type() == matador::data_type::type_varchar);
  REQUIRE(c3.as<std::string>() == "george");
  REQUIRE(c3.attributes().size() == 255);

  REQUIRE(c2.name().empty());
  REQUIRE(c2.index() == -1);
  REQUIRE(c2.ref_table().empty());
  REQUIRE(c2.ref_column().empty());
  REQUIRE(c2.type() == matador::data_type::type_varchar);
  REQUIRE(c2.as<std::string>().empty());
  REQUIRE(c2.attributes().size() == 255);
}