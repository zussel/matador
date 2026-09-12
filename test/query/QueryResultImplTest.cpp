#include <catch2/catch_test_macros.hpp>

#include "matador/query/access.hpp"
#include "matador/query/collection.hpp"
#include "matador/query/intermediates/fetchable_query.hpp"
#include "matador/query/internal/query_result_impl.hpp"
#include "matador/query/query_result.hpp"

#include <limits>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace {

using cell = std::variant<uint64_t, std::string>;

class result_reader final : public matador::query::query_result_reader {
public:
  explicit result_reader(std::vector<std::vector<cell>> rows, const size_t fail_on_fetch = no_failure)
  : rows_(std::move(rows))
  , fail_on_fetch_(fail_on_fetch) {}

  [[nodiscard]] size_t column_count() const override { return rows_.empty() ? 0 : rows_.front().size(); }
  [[nodiscard]] const char *column(size_t) const override { return ""; }

  [[nodiscard]] matador::result<bool, matador::utils::error> fetch() override {
    if (next_row_ == fail_on_fetch_) {
      return matador::failure<matador::utils::error>(matador::utils::error{});
    }
    if (next_row_ == rows_.size()) {
      return matador::ok<bool>(false);
    }
    current_row_ = next_row_++;
    return matador::ok<bool>(true);
  }

  [[nodiscard]] size_t start_column_index() const override { return 0; }

  void unshift() override {
    --next_row_;
  }

  void read_value(const char *, size_t index, uint64_t &value) override {
    value = std::get<uint64_t>(rows_.at(current_row_).at(index));
  }

  void read_value(const char *, size_t index, std::string &value) override {
    value = std::get<std::string>(rows_.at(current_row_).at(index));
  }

  void read_value(const char *, size_t index, std::string &value, size_t) override {
    read_value(nullptr, index, value);
  }

#define UNSUPPORTED_READER(Type) \
  void read_value(const char *, size_t, Type &) override { throw std::logic_error("unexpected value type"); }
  UNSUPPORTED_READER(int8_t)
  UNSUPPORTED_READER(int16_t)
  UNSUPPORTED_READER(int32_t)
  UNSUPPORTED_READER(int64_t)
  UNSUPPORTED_READER(uint8_t)
  UNSUPPORTED_READER(uint16_t)
  UNSUPPORTED_READER(uint32_t)
  UNSUPPORTED_READER(bool)
  UNSUPPORTED_READER(float)
  UNSUPPORTED_READER(double)
  UNSUPPORTED_READER(matador::utils::time_type_t)
  UNSUPPORTED_READER(matador::utils::date_type_t)
  UNSUPPORTED_READER(matador::utils::timestamp_type_t)
  UNSUPPORTED_READER(matador::utils::blob_type_t)
#undef UNSUPPORTED_READER

  void read_value(const char *, size_t, char *, size_t) override {
    throw std::logic_error("unexpected value type");
  }

  void read_value(const char *, size_t, matador::query::column_value &, size_t) override {
    throw std::logic_error("unexpected value type");
  }

protected:
  matador::query::value_reader &result_binder() override {
    return *this;
  }

private:
  static constexpr size_t no_failure = std::numeric_limits<size_t>::max();

  std::vector<std::vector<cell>> rows_;
  size_t fail_on_fetch_;
  size_t next_row_{};
  size_t current_row_{};
};

struct revision_model {
  uint64_t id{};
  uint64_t revision{};
  std::string name;

  template<class Operator>
  void process(Operator &op) {
    namespace access = matador::query::access;
    access::primary_key(op, "id", id);
    access::revision(op, "revision", revision);
    access::attribute(op, "name", name);
  }
};

struct child {
  uint64_t id{};
  std::string name;

  template<class Operator>
  void process(Operator &op) {
    namespace access = matador::query::access;
    access::primary_key(op, "child_id", id);
    access::attribute(op, "child_name", name);
  }
};

struct parent {
  uint64_t id{};
  matador::query::object_ptr<child> child_;

  template<class Operator>
  void process(Operator &op) {
    namespace access = matador::query::access;
    access::primary_key(op, "parent_id", id);
    access::has_one(op, "child", child_, "parent_id", matador::CascadeNoneFetchEager);
  }
};

struct collection_parent {
  uint64_t id{};
  matador::query::collection<matador::query::object_ptr<child>> children;

  template<class Operator>
  void process(Operator &op) {
    namespace access = matador::query::access;
    access::primary_key(op, "parent_id", id);
    access::has_many(op, "children", children, "parent_id", matador::CascadeNoneFetchEager);
  }
};

template<class Type>
matador::query::query_result_impl make_result(std::vector<std::vector<cell>> rows) {
  return {
    std::make_unique<result_reader>(std::move(rows)),
    {},
    std::make_shared<matador::query::resolver_service>(),
    typeid(Type)
  };
}

TEST_CASE("query_result_impl reads a revision once", "[query_result_impl]") {
  auto result = make_result<revision_model>({{{uint64_t{1}, uint64_t{7}, std::string{"first"}}}});
  revision_model model;

  REQUIRE(result.fetch(model));
  REQUIRE(model.id == 1);
  REQUIRE(model.revision == 7);
  REQUIRE(model.name == "first");
}

TEST_CASE("query_result_impl handles eager has_one rows", "[query_result_impl]") {
  auto result = make_result<parent>({
    {{uint64_t{1}, uint64_t{2}, std::string{"child"}}},
    {{uint64_t{3}, uint64_t{0}, std::string{""}}}
  });
  parent model;

  REQUIRE(result.fetch(model));
  REQUIRE(model.child_);
  REQUIRE(model.child_->id == 2);
  REQUIRE(model.child_->name == "child");

  model = {};
  REQUIRE(result.fetch(model));
  REQUIRE(model.child_ == matador::query::nullobj);
}

TEST_CASE("query_result_impl resets and deduplicates eager collections per parent", "[query_result_impl]") {
  auto result = make_result<collection_parent>({
    {{uint64_t{1}, uint64_t{2}, std::string{"first"}}},
    {{uint64_t{1}, uint64_t{2}, std::string{"first"}}},
    {{uint64_t{1}, uint64_t{3}, std::string{"second"}}},
    {{uint64_t{3}, uint64_t{4}, std::string{"second"}}}
  });
  collection_parent first;
  collection_parent second;

  REQUIRE(result.fetch(first));
  REQUIRE(first.children.size() == 2);
  auto child_it = first.children.begin();
  REQUIRE(child_it->primary_key().as<uint64_t>().value() == 2);
  ++child_it;
  REQUIRE(child_it->primary_key().as<uint64_t>().value() == 3);

  REQUIRE(result.fetch(second));
  REQUIRE(second.children.size() == 1);
  REQUIRE(second.children.begin()->primary_key().as<uint64_t>().value() == 4);
}

TEST_CASE("query_result_impl propagates reader failures", "[query_result_impl]") {
  matador::query::query_result_impl result(
    std::make_unique<result_reader>(std::vector<std::vector<cell>>{}, 0),
    {},
    std::make_shared<matador::query::resolver_service>(),
    typeid(revision_model)
  );
  revision_model model;

  REQUIRE_THROWS_AS(result.fetch(model), matador::utils::error_exception);
}

TEST_CASE("query_result_impl propagates reader failures for records", "[query_result_impl]") {
  matador::query::query_result_impl result(
    std::make_unique<result_reader>(std::vector<std::vector<cell>>{}, 0),
    {},
    std::make_shared<matador::query::resolver_service>(),
    typeid(matador::query::record)
  );
  matador::query::record record;

  REQUIRE_THROWS_AS(result.fetch(record), matador::utils::error_exception);
}

TEST_CASE("query_result iterates typed results", "[query_result]") {
  auto resolver_service = std::make_shared<matador::query::resolver_service>();
  auto result = matador::query::query_result<revision_model>::make_query_result(
    std::make_unique<matador::query::query_result_impl>(
      std::make_unique<result_reader>(std::vector<std::vector<cell>>{
        {{uint64_t{1}, uint64_t{7}, std::string{"first"}}},
        {{uint64_t{2}, uint64_t{8}, std::string{"second"}}}
      }),
      std::vector<matador::query::column>{},
      resolver_service,
      typeid(revision_model)
    ),
    resolver_service->resolver<revision_model>(),
    [] { return std::make_shared<revision_model>(); }
  );

  REQUIRE(result.is_ok());
  auto query_result = result.release();
  auto first = query_result.begin();
  REQUIRE(first != query_result.end());
  REQUIRE((*first)->id == 1);
  REQUIRE((*first)->name == "first");

  auto previous = first++;
  REQUIRE((*previous)->id == 1);
  REQUIRE((*first)->id == 2);
  REQUIRE((*first)->name == "second");

  ++first;
  REQUIRE(first == query_result.end());
}

TEST_CASE("query_result rejects invalid construction dependencies", "[query_result]") {
  auto resolver_service = std::make_shared<matador::query::resolver_service>();
  auto missing_impl = matador::query::query_result<revision_model>::make_query_result(
    std::unique_ptr<matador::query::query_result_impl>{},
    resolver_service->resolver<revision_model>(),
    [] { return std::make_shared<revision_model>(); }
  );

  REQUIRE(missing_impl.is_error());

  auto missing_creator = matador::query::query_result<revision_model>::make_query_result(
    std::make_unique<matador::query::query_result_impl>(
      std::make_unique<result_reader>(std::vector<std::vector<cell>>{}),
      std::vector<matador::query::column>{},
      resolver_service,
      typeid(revision_model)
    ),
    resolver_service->resolver<revision_model>(),
    std::function<std::shared_ptr<revision_model>()>{}
  );

  REQUIRE(missing_creator.is_error());
}

}

template matador::result<matador::query::query_result<revision_model>, matador::utils::error>
matador::query::fetchable_query::fetch_all<revision_model>(matador::query::executor &);

template matador::result<matador::query::object_ptr<revision_model>, matador::utils::error>
matador::query::fetchable_query::fetch_one<revision_model>(matador::query::executor &);
