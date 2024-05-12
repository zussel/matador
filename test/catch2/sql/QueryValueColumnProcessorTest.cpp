#include <catch2/catch_test_macros.hpp>

#include <matador/sql/query_value_column_processor.hpp>

using namespace matador;

TEST_CASE("Test query value column processor", "[query][column][convert][visitor]") {

  std::vector<sql::column_type> row_values {
    static_cast<char>(-56),
    static_cast<short>(-511),
    static_cast<int>(-2345123),
    static_cast<long>(-123456789),
    static_cast<long long>(-987654321),
    static_cast<unsigned char>(56),
    static_cast<unsigned short>(1023),
    static_cast<unsigned int>(2345123),
    static_cast<unsigned long>(123456789),
    static_cast<unsigned long long>(987654321),
    static_cast<bool>(true),
    static_cast<float>(-0.1234),
    static_cast<double>(-0.987654321),
    static_cast<const char*>("hello"),
    static_cast<std::string>("world"),
    static_cast<matador::date>(matador::date{}),
    static_cast<matador::time>(matador::time{}),
    matador::utils::blob{1, 2, 3, 4}
  };

  auto columns = std::make_shared<matador::columns>();

  detail::query_value_column_processor processor{columns};

  for (const auto &row_value : row_values) {
    auto p = std::pair<std::string, sql::column_type>("col", row_value);
    processor.execute(p);
  }

  REQUIRE(columns->columns_.size() == 18);
}