#include <catch2/catch_test_macros.hpp>

#include "matador/query/meta_table_macro.hpp"

#include <array>
#include <string>
#include <utility>

META_TABLE(meta_table_macro_types, META_TABLE_MACRO_TYPES_INSTANCE, id, name, age, color);
META_TABLE(meta_table_macro_single_column, META_TABLE_MACRO_SINGLE_COLUMN_INSTANCE, value);
META_TABLE(meta_table_macro_many_columns, META_TABLE_MACRO_MANY_COLUMNS_INSTANCE,
           id, val_char, val_float, val_double, val_short, val_int, val_long_long,
           val_unsigned_char, val_unsigned_short, val_unsigned_int, val_unsigned_long_long,
           val_bool, val_string, val_varchar, val_binary);

TEST_CASE("META_TABLE: supports many fields", "[query][meta-table]") {
  const auto& table = matador::query::meta::META_TABLE_MACRO_MANY_COLUMNS_INSTANCE;
  const std::array<std::string, 15> names{
    "id", "val_char", "val_float", "val_double", "val_short", "val_int", "val_long_long",
    "val_unsigned_char", "val_unsigned_short", "val_unsigned_int", "val_unsigned_long_long",
    "val_bool", "val_string", "val_varchar", "val_binary"
  };

  REQUIRE(table.columns().size() == names.size());
  for (std::size_t index = 0; index < names.size(); ++index) {
    REQUIRE(table.columns()[index].column_name() == names[index]);
  }
  REQUIRE(&table.id == &table.columns().front());
  REQUIRE(&table.val_binary == &table.columns().back());
}

TEST_CASE("META_TABLE: creates columns in declaration order", "[query][meta-table]") {
  const auto& types = matador::query::meta::META_TABLE_MACRO_TYPES_INSTANCE;
  const std::array<std::string, 4> expected_names{"id", "name", "age", "color"};

  REQUIRE(types.table_name() == "meta_table_macro_types");
  REQUIRE(types.columns().size() == expected_names.size());

  for (std::size_t index = 0; index < expected_names.size(); ++index) {
    REQUIRE(types.columns()[index].column_name() == expected_names[index]);
    REQUIRE(types.columns()[index].index() == index);
    REQUIRE(types.columns()[index].table() == &types);
  }

  REQUIRE(&types.id == &types.columns()[0]);
  REQUIRE(&types.name == &types.columns()[1]);
  REQUIRE(&types.age == &types.columns()[2]);
  REQUIRE(&types.color == &types.columns()[3]);
  REQUIRE(&matador::query::meta::META_TABLE_MACRO_TYPES_INSTANCE == &types);
}

TEST_CASE("META_TABLE: copy and move rebind field references", "[query][meta-table]") {
  using table_type = matador::query::meta::internal::meta_table_macro_types_table;

  const auto& original = matador::query::meta::META_TABLE_MACRO_TYPES_INSTANCE;
  const table_type copied{original};

  REQUIRE(&copied.id == &copied.columns()[0]);
  REQUIRE(&copied.name == &copied.columns()[1]);
  REQUIRE(&copied.age == &copied.columns()[2]);
  REQUIRE(&copied.color == &copied.columns()[3]);
  REQUIRE(copied.id.table() == &copied);
  REQUIRE(copied.id.table() != &original);

  table_type move_source{original};
  const table_type moved{std::move(move_source)};

  REQUIRE(&moved.id == &moved.columns()[0]);
  REQUIRE(&moved.name == &moved.columns()[1]);
  REQUIRE(&moved.age == &moved.columns()[2]);
  REQUIRE(&moved.color == &moved.columns()[3]);
  REQUIRE(moved.id.table() == &moved);
  REQUIRE(moved.id.table() != &original);
}

TEST_CASE("META_TABLE: supports a single field", "[query][meta-table]") {
  const auto& single = matador::query::meta::META_TABLE_MACRO_SINGLE_COLUMN_INSTANCE;

  REQUIRE(single.columns().size() == 1);
  REQUIRE(single.value.column_name() == "value");
  REQUIRE(&single.value == &single.columns()[0]);
  REQUIRE(single.value.table() == &single);
}