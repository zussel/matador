#include <catch2/catch_test_macros.hpp>

#include "matador/query/intermediates/select_intermediate.hpp"
#include "matador/query/internal/query_parts.hpp"
#include "matador/query/query_data.hpp"
#include "matador/query/table.hpp"

#include <stdexcept>
#include <string>
#include <vector>

using namespace matador::query;

namespace {
class inspectable_select_intermediate final : public select_intermediate {
public:
  using select_intermediate::select_intermediate;

  [[nodiscard]] const query_data& data() const {
    return *context_;
  }
};
}

TEST_CASE("Select intermediate stores the requested columns", "[query][select]") {
  const std::vector<column> columns{
    column::make_plain("id"),
    column::make_plain("name", "customer_name")
  };
  const inspectable_select_intermediate select{columns};

  REQUIRE(select.data().parts.size() == 1);
  const auto* part = dynamic_cast<const internal::query_select_part*>(select.data().parts.front().get());
  REQUIRE(part != nullptr);
  REQUIRE(part->columns().size() == 2);
  REQUIRE(part->columns().at(0).equals(columns.at(0)));
  REQUIRE(part->columns().at(1).equals(columns.at(1)));
}

TEST_CASE("Select intermediate replaces the projection with sequence expressions", "[query][select]") {
  SECTION("nextval") {
    inspectable_select_intermediate select{{column::make_plain("id")}};
    [[maybe_unused]] const auto query = select.nextval("customer_id_seq");

    REQUIRE(select.data().parts.size() == 1);
    const auto* part = dynamic_cast<const internal::query_select_nextval_part*>(
      select.data().parts.front().get()
    );
    REQUIRE(part != nullptr);
    REQUIRE(part->sequence_name() == "customer_id_seq");
  }

  SECTION("currval") {
    inspectable_select_intermediate select{{column::make_plain("id")}};
    [[maybe_unused]] const auto query = select.currval("customer_id_seq");

    REQUIRE(select.data().parts.size() == 1);
    const auto* part = dynamic_cast<const internal::query_select_currval_part*>(
      select.data().parts.front().get()
    );
    REQUIRE(part != nullptr);
    REQUIRE(part->sequence_name() == "customer_id_seq");
  }
}

TEST_CASE("Select intermediate appends FROM tables and records their query names", "[query][select]") {
  inspectable_select_intermediate select{{column::make_plain("id")}};
  const table customers{"customers"};
  const table orders{"orders"};

  [[maybe_unused]] const auto query = select.from(customers, orders);

  REQUIRE(select.data().parts.size() == 2);
  const auto* part = dynamic_cast<const internal::query_from_part*>(select.data().parts.back().get());
  REQUIRE(part != nullptr);
  REQUIRE(part->tables().size() == 2);
  REQUIRE(part->tables().at(0) == customers);
  REQUIRE(part->tables().at(1) == orders);
  REQUIRE(select.data().tables.at("customers") == customers);
  REQUIRE(select.data().tables.at("orders") == orders);
}

TEST_CASE("Select intermediate rejects an empty FROM clause", "[query][select]") {
  inspectable_select_intermediate select{{column::make_plain("id")}};

  REQUIRE_THROWS_AS(select.from(), std::invalid_argument);
}
