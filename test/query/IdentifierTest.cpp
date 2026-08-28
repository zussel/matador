#include <catch2/catch_test_macros.hpp>

#include "matador/query/identifier.hpp"
#include "matador/utils/default_type_traits.hpp"
#include "matador/query/column_value.hpp"

using namespace matador::query;
using namespace matador::utils;

TEST_CASE("Identifier: Test create identifier", "[identifier][create]") {
  const identifier id;

  REQUIRE(id.is_null());
  REQUIRE(!id.is_integer());
  REQUIRE(!id.is_varchar());
  REQUIRE(!id.is_valid());
  REQUIRE(id.str() == "null");
}

TEST_CASE("Identifier: Test assign value to identifier", "[identifier][assign]") {
  identifier id;

  REQUIRE(id.is_null());
  REQUIRE(!id.is_valid());
  REQUIRE(id.str() == "null");

  id = 7;

  REQUIRE(!id.is_null());
  REQUIRE(id.is_valid());
  REQUIRE(id.is_integer());
  REQUIRE(id.str() == "7");
  REQUIRE(id.type() == basic_type::Int32);
  REQUIRE(id.type_index() == std::type_index(typeid(int)));

  id = std::string{"UniqueId"};

  REQUIRE(!id.is_null());
  REQUIRE(id.is_valid());
  REQUIRE(id.is_varchar());
  REQUIRE(id.str() == "UniqueId");

  id = "UniqueId";

  REQUIRE(!id.is_null());
  REQUIRE(id.is_valid());
  REQUIRE(id.is_varchar());

  // REQUIRE(id == identifier{"UniqueId"});
}

TEST_CASE("Identifier: Test compare identifier", "[identifier][compare]") {
  identifier id1{6}, id2{7};

  REQUIRE(id1 != id2);
  REQUIRE(id1 < id2);
  REQUIRE(id1 <= id2);
  REQUIRE(id2 > id1);
  REQUIRE(id2 >= id1);
  REQUIRE(id1.hash() != id2.hash());

  id2 = 6;

  REQUIRE(id1 == id2);
  REQUIRE(id1.hash() == id2.hash());
}

identifier create(const int id) {
  return identifier{id};
}

TEST_CASE("Identifier: Test copy identifier" "[identifier][copy]") {
  identifier id1{"Unique"};
  REQUIRE(id1.is_valid());
  REQUIRE(id1.str() == "Unique");

  const identifier id2(id1);

  REQUIRE(id1 == id2);
  REQUIRE(id1.hash() == id2.hash());

  id1.clear();

  REQUIRE(id1.is_null());
  identifier id3 = id1;

  REQUIRE(id1 == id3);
  REQUIRE(!(id1 < id3));
  REQUIRE(id3.is_null());
  REQUIRE(id1.hash() == id3.hash());

  id3 = create(9);
  id3 = id1;
}

TEST_CASE("Identifier: Test move identifier", "[identifier][move]") {
  identifier id1{6};

  REQUIRE(id1.is_integer());
  REQUIRE(!id1.is_null());

  const auto id2 = std::move(id1);
  REQUIRE(!id1.is_null());
  REQUIRE(id2.is_integer());
}

TEST_CASE("Identifier: Identifier assignment from integer types", "[utils][identifier][assign]") {
  identifier id;

  id = int8_t{-5};
  REQUIRE(id.type() == basic_type::Int8);
  REQUIRE(id.str() == "-5");
  REQUIRE(id.is_integer());
  REQUIRE(id.is_valid());

  id = int16_t{42};
  REQUIRE(id.type() == basic_type::Int16);
  REQUIRE(id.str() == "42");
  REQUIRE(id.is_integer());
  REQUIRE(id.is_valid());

  id = uint32_t{123456u};
  REQUIRE(id.type() == basic_type::UInt32);
  REQUIRE(id.str() == "123456");
  REQUIRE(id.is_integer());
  REQUIRE(id.is_valid());
}

TEST_CASE("Identifier: Identifier assignment from string type", "[utils][identifier][assign]") {
  identifier id;

  id = std::string{"hello"};
  REQUIRE(id.type() == basic_type::Varchar);
  REQUIRE(id.str() == "hello");
  REQUIRE(id.is_varchar());
  REQUIRE(id.is_valid());

  id = std::string{};
  REQUIRE(id.type() == basic_type::Varchar);
  REQUIRE(id.str().empty());
  REQUIRE_FALSE(id.is_valid());
}

TEST_CASE("Identifier: Identifier assignment from const char*", "[utils][identifier][assign]") {
  identifier id;

  id = "world";
  REQUIRE(id.type() == basic_type::Varchar);
  REQUIRE(id.str() == "world");
  REQUIRE(id.is_varchar());
  REQUIRE(id.is_valid());

  id = static_cast<const char*>(nullptr);
  REQUIRE(id.type() == basic_type::Varchar);
  REQUIRE(id.str().empty());
  REQUIRE_FALSE(id.is_valid());
}

TEST_CASE("Identifier: Identifier assignment from nullptr", "[utils][identifier][assign]") {
  identifier id{42};

  REQUIRE(id.is_valid());
  REQUIRE_FALSE(id.is_null());

  id = nullptr;
  REQUIRE(id.is_null());
  REQUIRE(id.type() == basic_type::Null);
  REQUIRE(id.str() == "null");
  REQUIRE_FALSE(id.is_valid());
}

TEST_CASE("Identifier: Identifier reassignment between types", "[utils][identifier][assign]") {
  identifier id{uint64_t{7}};

  REQUIRE(id.is_integer());
  REQUIRE(id.str() == "7");

  id = std::string{"abc"};
  REQUIRE(id.is_varchar());
  REQUIRE(id.str() == "abc");
  REQUIRE(id.is_valid());

  id = int64_t{99};
  REQUIRE(id.is_integer());
  REQUIRE(id.str() == "99");
  REQUIRE(id.is_valid());

  id = nullptr;
  REQUIRE(id.is_null());
  REQUIRE(id.str() == "null");
}

TEST_CASE("Identifier: Identifier as() returns exact integer type", "[utils][identifier][as]") {
  identifier id{int32_t{42}};

  const auto as_i32 = id.as<int32_t>();
  REQUIRE(as_i32.is_ok());
  REQUIRE(*as_i32 == 42);

  const auto as_i64 = id.as<int64_t>();
  REQUIRE(as_i64.is_error());
}

TEST_CASE("Identifier: Identifier as() returns exact string type", "[utils][identifier][as]") {
  identifier id{std::string{"hello"}};

  const auto as_string = id.as<std::string>();
  REQUIRE(as_string.is_ok());
  REQUIRE(*as_string == "hello");

  const auto as_i32 = id.as<int32_t>();
  REQUIRE(as_i32.is_error());
}

TEST_CASE("Identifier: Identifier as() returns null type failure for null identifier", "[utils][identifier][as]") {
  identifier id{nullptr};

  const auto as_i32 = id.as<int32_t>();
  REQUIRE(as_i32.is_error());

  const auto as_string = id.as<std::string>();
  REQUIRE(as_string.is_error());
}

TEST_CASE("Identifier: Identifier convert() converts between integer types", "[utils][identifier][convert]") {
  identifier id{int32_t{123}};

  const auto as_i64 = id.convert<int64_t>();
  REQUIRE(as_i64.is_ok());
  REQUIRE(*as_i64 == 123);

  const auto as_u8 = id.convert<uint8_t>();
  REQUIRE(as_u8.is_ok());
  REQUIRE(*as_u8 == 123);
}

TEST_CASE("Identifier: Identifier convert() rejects out of range integer conversion", "[utils][identifier][convert]") {
  identifier id{int32_t{300}};

  const auto as_u8 = id.convert<uint8_t>();
  REQUIRE(as_u8.is_error());
}

TEST_CASE("Identifier: Identifier convert() rejects negative to unsigned conversion", "[utils][identifier][convert]") {
  identifier id{int32_t{-1}};

  const auto as_u32 = id.convert<uint32_t>();
  REQUIRE(as_u32.is_error());
}

TEST_CASE("Identifier: Identifier convert() rejects non-integer source types", "[utils][identifier][convert]") {
  identifier id{std::string{"123"}};

  const auto as_i32 = id.convert<int32_t>();
  REQUIRE(as_i32.is_error());

  identifier null_id{nullptr};
  const auto null_to_i64 = null_id.convert<int64_t>();
  REQUIRE(null_to_i64.is_error());
}

TEST_CASE("Identifier: Identifier convert() works with unsigned integer sources", "[utils][identifier][convert]") {
  identifier id{uint16_t{500}};

  const auto as_i32 = id.convert<int32_t>();
  REQUIRE(as_i32.is_ok());
  REQUIRE(*as_i32 == 500);

  const auto as_u8 = id.convert<uint8_t>();
  REQUIRE(as_u8.is_error());
}

TEST_CASE("Identifier: Identifier as() and convert() behave consistently", "[utils][identifier][as][convert]") {
  identifier id{uint64_t{77}};

  const auto as_u64 = id.as<uint64_t>();
  REQUIRE(as_u64.is_ok());
  REQUIRE(*as_u64 == 77);

  const auto conv_u64 = id.convert<uint64_t>();
  REQUIRE(conv_u64.is_ok());
  REQUIRE(*conv_u64 == 77);
}

TEST_CASE("Identifier: Identifier assign from value", "[utils][identifier][assign]") {
  identifier id;

  column_value v1{int32_t{17}};
  const auto r1 = id.assign(v1);
  REQUIRE(r1.is_ok());
  REQUIRE(id.is_integer());
  REQUIRE(id.str() == "17");

  column_value v2{std::string{"abc"}};
  const auto r2 = id.assign(v2);
  REQUIRE(r2.is_ok());
  REQUIRE(id.is_varchar());
  REQUIRE(id.str() == "abc");

  column_value v3{basic_type::Double, 0};
  const auto r3 = id.assign(v3);
  REQUIRE(r3.is_error());
}

TEST_CASE("Identifier: Identifier assign from value with exact type mapping", "[utils][identifier][assign]") {
  identifier id;

  SECTION("integer types") {
    column_value v{int16_t{12}};
    const auto r = id.assign(v);
    REQUIRE(r.is_ok());
    REQUIRE(id.is_integer());
    REQUIRE(id.str() == "12");
  }

  SECTION("string types") {
    column_value v{std::string{"abc"}};
    const auto r = id.assign(v);
    REQUIRE(r.is_ok());
    REQUIRE(id.is_varchar());
    REQUIRE(id.str() == "abc");
  }

  SECTION("null") {
    column_value v{basic_type::Null, 0};
    const auto r = id.assign(v);
    REQUIRE(r.is_ok());
    REQUIRE(id.is_null());
  }
}

TEST_CASE("Identifier: Identifier::to_database_type converts internal value", "[identifier][to_database_type]") {
  SECTION("null identifier becomes nullptr") {
    identifier id{nullptr};

    const auto db_value = id.to_database_type();

    REQUIRE(std::holds_alternative<std::nullptr_t>(db_value));
  }

  SECTION("Identifier: Signed integral identifier becomes same signed database type") {
    identifier id{int32_t{42}};

    const auto db_value = id.to_database_type();

    REQUIRE(std::holds_alternative<int32_t>(db_value));
    REQUIRE(std::get<int32_t>(db_value) == 42);
  }

  SECTION("Identifier: Unsigned integral identifier becomes same unsigned database type") {
    identifier id{uint64_t{123456789ULL}};

    const auto db_value = id.to_database_type();

    REQUIRE(std::holds_alternative<uint64_t>(db_value));
    REQUIRE(std::get<uint64_t>(db_value) == 123456789ULL);
  }

  SECTION("Identifier: String identifier becomes std::string database type") {
    identifier id{std::string{"customer_1"}};

    const auto db_value = id.to_database_type();

    REQUIRE(std::holds_alternative<std::string>(db_value));
    REQUIRE(std::get<std::string>(db_value) == "customer_1");
  }
}