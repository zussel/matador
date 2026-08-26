#include <catch2/catch_test_macros.hpp>

#include "matador/utils/string.hpp"

using namespace matador::utils;

TEST_CASE("Test splitting of string", "[string][split]") {
  const std::string str("1,2,3,5,6");

  std::vector<std::string> string_vec;

  const size_t count = split(str, ',', string_vec);

  REQUIRE(count == 5);
  REQUIRE(string_vec.size() == 5);
}

TEST_CASE("Test trimming of string", "[string][trim]") {
  std::string str("   middle     ");

  std::string result = trim(str);

  REQUIRE(result == "middle");

  result = trim(str, "-");

  REQUIRE(result == str);
}
