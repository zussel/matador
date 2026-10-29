#include <catch2/catch_test_macros.hpp>

#include "matador/utils/data_type.hpp"

#include <array>
#include <cstddef>

using namespace matador;
using namespace matador::utils;

namespace {

struct fixed_data_type_case {
  data_type descriptor;
  basic_type expected_type{};
};

struct sized_data_type_case {
  data_type descriptor;
  basic_type expected_type{};
  std::size_t expected_size{};
};

} // namespace

TEST_CASE("Data type: fixed aliases expose their basic type and zero size", "[utils][data_type]") {
  const std::array<fixed_data_type_case, 18> cases{{
    {TinyInt{}, basic_type::Int8},
    {SmallInt{}, basic_type::Int16},
    {Integer{}, basic_type::Int32},
    {BigInt{}, basic_type::Int64},
    {TinyIntUnsigned{}, basic_type::UInt8},
    {SmallIntUnsigned{}, basic_type::UInt16},
    {IntegerUnsigned{}, basic_type::UInt32},
    {BigIntUnsigned{}, basic_type::UInt64},
    {Real{}, basic_type::Float},
    {Double{}, basic_type::Double},
    {Text{}, basic_type::Text},
    {Boolean{}, basic_type::Boolean},
    {Time{}, basic_type::Time},
    {Date{}, basic_type::Date},
    {Timestamp{}, basic_type::DateTime},
    {Null{}, basic_type::Null},
    {data_type{basic_type::Unknown}, basic_type::Unknown},
    {data_type{basic_type::Varchar}, basic_type::Varchar},
  }};

  for (const auto &[descriptor, expected_type] : cases) {
    REQUIRE(descriptor.type() == expected_type);
    REQUIRE(descriptor.size() == 0);
  }
}

TEST_CASE("Data type: sized aliases expose their basic type and declared size", "[utils][data_type]") {
  const std::array<sized_data_type_case, 2> cases{{
    {Varchar{255}, basic_type::Varchar, 255},
    {Blob{4096}, basic_type::Blob, 4096},
  }};

  for (const auto& test_case : cases) {
    REQUIRE(test_case.descriptor.type() == test_case.expected_type);
    REQUIRE(test_case.descriptor.size() == test_case.expected_size);
  }
}

TEST_CASE("Data type: aliases convert to data_type descriptors", "[utils][data_type]") {
  constexpr data_type tiny_int = TinyInt{};
  REQUIRE(tiny_int.type() == basic_type::Int8);
  REQUIRE(tiny_int.size() == 0);

  constexpr data_type varchar = Varchar{128};
  REQUIRE(varchar.type() == basic_type::Varchar);
  REQUIRE(varchar.size() == 128);

  constexpr data_type blob = Blob{1024};
  REQUIRE(blob.type() == basic_type::Blob);
  REQUIRE(blob.size() == 1024);
}

TEST_CASE("Data type: equality compares type and size", "[utils][data_type]") {
  REQUIRE(TinyInt{} == TinyInt{});
  REQUIRE_FALSE(TinyInt{} != TinyInt{});

  REQUIRE(Varchar{64} == Varchar{64});
  REQUIRE_FALSE(Varchar{64} != Varchar{64});

  REQUIRE(Varchar{64} != Varchar{128});
  REQUIRE_FALSE(Varchar{64} == Varchar{128});

  REQUIRE(Varchar{64} != Blob{64});
  REQUIRE_FALSE(Varchar{64} == Blob{64});

  REQUIRE(TinyInt{} == data_type{basic_type::Int8});
  REQUIRE(data_type{basic_type::Int8} == TinyInt{});
  REQUIRE(Varchar{64} == data_type{basic_type::Varchar, 64});
  REQUIRE(data_type{basic_type::Varchar, 64} == Varchar{64});
}

TEST_CASE("Data type: ordering compares basic type first and size second", "[utils][data_type]") {
  REQUIRE(TinyInt{} < SmallInt{});
  REQUIRE(TinyInt{} <= SmallInt{});
  REQUIRE(SmallInt{} > TinyInt{});
  REQUIRE(SmallInt{} >= TinyInt{});

  REQUIRE(Varchar{64} < Varchar{128});
  REQUIRE(Varchar{64} <= Varchar{128});
  REQUIRE(Varchar{128} > Varchar{64});
  REQUIRE(Varchar{128} >= Varchar{64});

  REQUIRE_FALSE(Varchar{128} < Varchar{64});
  REQUIRE_FALSE(Varchar{64} > Varchar{128});

  REQUIRE(Varchar{64} <= Varchar{64});
  REQUIRE(Varchar{64} >= Varchar{64});

  REQUIRE(data_type{basic_type::Int8} < SmallInt{});
  REQUIRE(TinyInt{} < data_type{basic_type::Int16});
  REQUIRE(data_type{basic_type::Varchar, 64} < Varchar{128});
  REQUIRE(Varchar{64} < data_type{basic_type::Varchar, 128});
}

TEST_CASE("Data type: descriptors are constexpr friendly", "[utils][data_type]") {
  constexpr data_type tiny_int = TinyInt{};
  static_assert(tiny_int.type() == basic_type::Int8);
  static_assert(tiny_int.size() == 0);

  constexpr data_type varchar = Varchar{255};
  static_assert(varchar.type() == basic_type::Varchar);
  static_assert(varchar.size() == 255);

  static_assert(TinyInt{} == data_type{basic_type::Int8});
  static_assert(Varchar{32} == data_type{basic_type::Varchar, 32});
  static_assert(Varchar{32} != Varchar{64});
  static_assert(TinyInt{} < SmallInt{});
  static_assert(Varchar{32} < Varchar{64});

  REQUIRE(tiny_int == TinyInt{});
  REQUIRE(varchar == Varchar{255});
}