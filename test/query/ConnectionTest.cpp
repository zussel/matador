#include <catch2/catch_test_macros.hpp>

#include "matador/query/backend_provider.hpp"
#include "matador/query/connection.hpp"
#include "matador/query/dialect_builder.hpp"
#include "matador/query/error_code.hpp"
#include "matador/query/interface/connection_impl.hpp"
#include "matador/query/interface/statement_impl.hpp"
#include "matador/query/query_context.hpp"
#include "matador/query/value_writer.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace {

using namespace matador;
using namespace matador::query;

constexpr auto test_backend_type = "connection_test_backend";

struct fake_backend_state {
  size_t create_count{};
  size_t destroy_count{};
  size_t open_count{};
  size_t close_count{};
  size_t is_open_count{};
  size_t execute_count{};
  size_t fetch_count{};
  size_t prepare_count{};
  size_t describe_count{};
  size_t exists_count{};
  size_t sequence_exists_count{};

  bool open{};
  bool fail_open{};
  bool fail_close{};
  bool fail_is_open{};
  bool fail_execute{};
  bool fail_fetch{};
  bool fail_prepare{};

  std::vector<std::string> executed_sql;
  std::vector<std::string> fetched_sql;
  std::vector<std::string> prepared_sql;

  std::string last_describe_table;
  std::string last_exists_schema;
  std::string last_exists_table;
  std::string last_sequence_schema;
  std::string last_sequence_name;
};

fake_backend_state &state() {
  static fake_backend_state instance;
  return instance;
}

utils::error test_error(const std::string &message) {
  return utils::error{error_code::Failure, message};
}

class recording_logger final : public abstract_sql_logger {
public:
  void on_connect() override { ++connect_count; }
  void on_close() override { ++close_count; }
  void on_execute(const std::string &stmt) override {
    ++execute_count;
    executed_sql.push_back(stmt);
  }
  void on_fetch(const std::string &stmt) override {
    ++fetch_count;
    fetched_sql.push_back(stmt);
  }
  void on_prepare(const std::string &stmt) override {
    ++prepare_count;
    prepared_sql.push_back(stmt);
  }
  void on_error(const utils::error &) override { ++error_count; }

  size_t connect_count{};
  size_t close_count{};
  size_t execute_count{};
  size_t fetch_count{};
  size_t prepare_count{};
  size_t error_count{};

  std::vector<std::string> executed_sql;
  std::vector<std::string> fetched_sql;
  std::vector<std::string> prepared_sql;
};

class fake_value_writer final : public value_writer {
public:
  void write_value(size_t, const int8_t &) override {}
  void write_value(size_t, const int16_t &) override {}
  void write_value(size_t, const int32_t &) override {}
  void write_value(size_t, const int64_t &) override {}
  void write_value(size_t, const uint8_t &) override {}
  void write_value(size_t, const uint16_t &) override {}
  void write_value(size_t, const uint32_t &) override {}
  void write_value(size_t, const uint64_t &) override {}
  void write_value(size_t, const bool &) override {}
  void write_value(size_t, const float &) override {}
  void write_value(size_t, const double &) override {}
  void write_value(size_t, const utils::date_type_t &) override {}
  void write_value(size_t, const utils::time_type_t &) override {}
  void write_value(size_t, const utils::timestamp_type_t &) override {}
  void write_value(size_t, const char *) override {}
  void write_value(size_t, const char *, size_t) override {}
  void write_value(size_t, const std::string &) override {}
  void write_value(size_t, const std::string &, size_t) override {}
  void write_value(size_t, const utils::blob_type_t &) override {}
  void write_value(size_t, const column_value &, size_t) override {}
};

class fake_statement_impl final : public statement_impl {
public:
  explicit fake_statement_impl(query_context ctx)
  : statement_impl(std::move(ctx), 0) {}

  utils::result<execute_result, utils::error> execute(const value_writer &) override {
    return ok<execute_result>(execute_result{7, {}});
  }

  utils::result<std::unique_ptr<query_result_impl>, utils::error> fetch(const value_writer &) override {
    return ok<std::unique_ptr<query_result_impl>>();
  }

protected:
  [[nodiscard]] std::unique_ptr<value_writer> create_binder() const override {
    return std::make_unique<fake_value_writer>();
  }
};

class fake_connection_impl final : public connection_impl {
public:
  explicit fake_connection_impl(const connection_info &info)
  : connection_impl(info) {}

  utils::result<void, utils::error> open() override {
    ++state().open_count;
    if (state().fail_open) {
      return utils::failure(test_error("open failed"));
    }

    state().open = true;
    return utils::ok<void>();
  }

  utils::result<void, utils::error> close() override {
    ++state().close_count;
    if (state().fail_close) {
      return utils::failure(test_error("close failed"));
    }

    state().open = false;
    return utils::ok<void>();
  }

  utils::result<bool, utils::error> is_open() const override {
    ++state().is_open_count;
    if (state().fail_is_open) {
      return utils::failure(test_error("is_open failed"));
    }

    return utils::ok<bool>(state().open);
  }

  utils::result<bool, utils::error> is_valid() const override {
    return utils::ok<bool>(true);
  }

  utils::result<utils::version, utils::error> client_version() const override {
    return ok<utils::version>();
  }

  utils::result<utils::version, utils::error> server_version() const override {
    return ok<utils::version>();
  }

  utils::result<execute_result, utils::error> execute(const query_context &context) override {
    ++state().execute_count;
    state().executed_sql.push_back(context.sql);

    if (state().fail_execute) {
      return utils::failure(test_error("execute failed"));
    }

    return utils::ok<execute_result>(execute_result{42, {}});
  }

  utils::result<std::unique_ptr<query_result_impl>, utils::error> fetch(const query_context &context) override {
    ++state().fetch_count;
    state().fetched_sql.push_back(context.sql);

    if (state().fail_fetch) {
      return utils::failure(test_error("fetch failed"));
    }

    return ok<std::unique_ptr<query_result_impl>>();
  }

  utils::result<std::unique_ptr<statement_impl>, utils::error> prepare(const query_context &context) override {
    ++state().prepare_count;
    state().prepared_sql.push_back(context.sql);

    if (state().fail_prepare) {
      return utils::failure(test_error("prepare failed"));
    }

    return utils::ok<std::unique_ptr<statement_impl>>(
      std::make_unique<fake_statement_impl>(context));
  }

  utils::result<std::vector<column>, utils::error> describe(const std::string &table) override {
    ++state().describe_count;
    state().last_describe_table = table;
    return ok<std::vector<column>>();
  }

  utils::result<bool, utils::error> exists(const std::string &schema_name,
                                           const std::string &table_name) override {
    ++state().exists_count;
    state().last_exists_schema = schema_name;
    state().last_exists_table = table_name;
    return utils::ok<bool>(true);
  }

  utils::result<bool, utils::error> sequence_exists(const std::string &schema_name,
                                                    const std::string &sequence_name) override {
    ++state().sequence_exists_count;
    state().last_sequence_schema = schema_name;
    state().last_sequence_name = sequence_name;
    return utils::ok<bool>(true);
  }

  std::string to_escaped_string(const utils::blob_type_t &) const override {
    return {};
  }
};

class fake_backend_service final : public backend_provider::basic_backend_service {
public:
  fake_backend_service()
  : dialect_(dialect_builder::builder()
               .create()
               .with_default_schema_name("default_schema")
               .build()) {}

  connection_impl *create(const connection_info &info) override {
    ++state().create_count;
    return new fake_connection_impl(info);
  }

  void destroy(connection_impl *conn) override {
    ++state().destroy_count;
    delete conn;
  }

  [[nodiscard]] const query::dialect *dialect() const override {
    return &dialect_;
  }

private:
  query::dialect dialect_;
};

void ensure_backend_registered() {
  static const bool registered = [] {
    backend_provider::instance().register_backend(
      test_backend_type,
      std::make_unique<fake_backend_service>());
    return true;
  }();

  static_cast<void>(registered);
}

void reset_state() {
  state() = fake_backend_state{};
}

connection_info make_connection_info() {
  connection_info info;
  info.type = test_backend_type;
  return info;
}

query_context make_context(std::string sql, const query_command command = query_command::Unknown) {
  query_context ctx;
  ctx.sql = std::move(sql);
  ctx.command = command;
  return ctx;
}

} // namespace

TEST_CASE("connection creates and destroys backend connection", "[connection]") {
  ensure_backend_registered();
  reset_state();

  {
    const connection conn(make_connection_info());

    REQUIRE(conn.type() == test_backend_type);
    REQUIRE(conn.info().type == test_backend_type);
    REQUIRE(state().create_count == 1);
    REQUIRE(state().destroy_count == 0);
  }

  REQUIRE(state().destroy_count == 1);
}

TEST_CASE("connection can be constructed from dns string", "[connection]") {
  ensure_backend_registered();
  reset_state();

  const connection conn(std::string{test_backend_type} + "://test");

  REQUIRE(conn.type() == test_backend_type);
  REQUIRE(state().create_count == 1);
}

TEST_CASE("connection open is idempotent and logs only when opening", "[connection]") {
  ensure_backend_registered();
  reset_state();

  auto logger = std::make_shared<recording_logger>();
  const connection conn(make_connection_info(), logger);

  auto first_open = conn.open();
  REQUIRE(first_open.is_ok());
  REQUIRE(state().open);
  REQUIRE(state().open_count == 1);
  REQUIRE(logger->connect_count == 1);

  auto second_open = conn.open();
  REQUIRE(second_open.is_ok());
  REQUIRE(state().open_count == 1);
  REQUIRE(logger->connect_count == 1);
}

TEST_CASE("connection close closes only an open connection", "[connection]") {
  ensure_backend_registered();
  reset_state();

  auto logger = std::make_shared<recording_logger>();
  const connection conn(make_connection_info(), logger);

  auto close_when_closed = conn.close();
  REQUIRE(close_when_closed.is_ok());
  REQUIRE(state().close_count == 0);
  REQUIRE(logger->close_count == 0);

  REQUIRE(conn.open().is_ok());

  auto close_when_open = conn.close();
  REQUIRE(close_when_open.is_ok());
  REQUIRE_FALSE(state().open);
  REQUIRE(state().close_count == 1);
  REQUIRE(logger->close_count == 1);
}

TEST_CASE("connection destructor closes an open connection before destroying it", "[connection]") {
  ensure_backend_registered();
  reset_state();

  {
    const connection conn(make_connection_info());
    REQUIRE(conn.open().is_ok());
    REQUIRE(state().open);
  }

  REQUIRE_FALSE(state().open);
  REQUIRE(state().close_count == 1);
  REQUIRE(state().destroy_count == 1);
}

TEST_CASE("connection destructor logs close errors but still destroys backend connection", "[connection]") {
  ensure_backend_registered();
  reset_state();

  auto logger = std::make_shared<recording_logger>();

  {
    const connection conn(make_connection_info(), logger);
    REQUIRE(conn.open().is_ok());

    state().fail_close = true;
  }

  REQUIRE(logger->error_count == 1);
  REQUIRE(state().destroy_count == 1);
}

TEST_CASE("connection open propagates is_open and open errors", "[connection]") {
  ensure_backend_registered();

  SECTION("is_open failure") {
    reset_state();

    const connection conn(make_connection_info());
    state().fail_is_open = true;

    const auto result = conn.open();

    REQUIRE(result.is_error());
    REQUIRE(state().open_count == 0);
  }

  SECTION("open failure") {
    reset_state();

    const connection conn(make_connection_info());
    state().fail_open = true;

    const auto result = conn.open();

    REQUIRE(result.is_error());
    REQUIRE(state().open_count == 1);
    REQUIRE_FALSE(state().open);
  }
}

TEST_CASE("connection close propagates is_open errors", "[connection]") {
  ensure_backend_registered();
  reset_state();

  const connection conn(make_connection_info());
  state().fail_is_open = true;

  const auto result = conn.close();

  REQUIRE(result.is_error());
  REQUIRE(state().close_count == 0);
}

TEST_CASE("connection execute delegates to backend and logs sql", "[connection]") {
  ensure_backend_registered();
  reset_state();

  auto logger = std::make_shared<recording_logger>();
  const connection conn(make_connection_info(), logger);

  const auto result = conn.execute(make_context("DELETE FROM person"));

  REQUIRE(result.is_ok());
  REQUIRE(result->affected_rows == 42);
  REQUIRE(state().execute_count == 1);
  REQUIRE(state().executed_sql == std::vector<std::string>{"DELETE FROM person"});
  REQUIRE(logger->execute_count == 1);
  REQUIRE(logger->executed_sql == std::vector<std::string>{"DELETE FROM person"});
}

TEST_CASE("connection execute propagates backend errors", "[connection]") {
  ensure_backend_registered();
  reset_state();

  const connection conn(make_connection_info());
  state().fail_execute = true;

  const auto result = conn.execute(make_context("BROKEN SQL"));

  REQUIRE(result.is_error());
  REQUIRE(state().execute_count == 1);
}

TEST_CASE("connection fetch delegates to backend and logs sql", "[connection]") {
  ensure_backend_registered();
  reset_state();

  auto logger = std::make_shared<recording_logger>();
  const connection conn(make_connection_info(), logger);

  const auto result = conn.fetch(make_context("SELECT * FROM person"));

  REQUIRE(result.is_ok());
  REQUIRE(state().fetch_count == 1);
  REQUIRE(state().fetched_sql == std::vector<std::string>{"SELECT * FROM person"});
  REQUIRE(logger->fetch_count == 1);
  REQUIRE(logger->fetched_sql == std::vector<std::string>{"SELECT * FROM person"});
}

TEST_CASE("connection prepare delegates to backend and logs sql", "[connection]") {
  ensure_backend_registered();
  reset_state();

  auto logger = std::make_shared<recording_logger>();
  const connection conn(make_connection_info(), logger);

  auto result = conn.prepare(make_context("INSERT INTO person(name) VALUES (?)"));

  REQUIRE(result.is_ok());
  REQUIRE(state().prepare_count == 1);
  REQUIRE(state().prepared_sql == std::vector<std::string>{"INSERT INTO person(name) VALUES (?)"});
  REQUIRE(logger->prepare_count == 1);
  REQUIRE(logger->prepared_sql == std::vector<std::string>{"INSERT INTO person(name) VALUES (?)"});

  auto stmt = result.release();
  REQUIRE(stmt.sql() == "INSERT INTO person(name) VALUES (?)");

  const auto execute_result = stmt.execute();
  REQUIRE(execute_result.is_ok());
  REQUIRE(execute_result->affected_rows == 7);
}

TEST_CASE("connection prepare propagates backend errors", "[connection]") {
  ensure_backend_registered();
  reset_state();

  const connection conn(make_connection_info());
  state().fail_prepare = true;

  const auto result = conn.prepare(make_context("BROKEN PREPARE"));

  REQUIRE(result.is_error());
  REQUIRE(state().prepare_count == 1);
}

TEST_CASE("connection transaction helpers execute dialect transaction statements", "[connection]") {
  ensure_backend_registered();
  reset_state();

  auto logger = std::make_shared<recording_logger>();
  const connection conn(make_connection_info(), logger);

  REQUIRE(conn.begin().is_ok());
  REQUIRE(conn.commit().is_ok());
  REQUIRE(conn.rollback().is_ok());

  REQUIRE(state().executed_sql.size() == 3);
  REQUIRE(state().executed_sql[0] == conn.dialect().begin());
  REQUIRE(state().executed_sql[1] == conn.dialect().commit());
  REQUIRE(state().executed_sql[2] == conn.dialect().rollback());

  REQUIRE(logger->execute_count == 3);
}

TEST_CASE("connection transaction helpers propagate execute errors", "[connection]") {
  ensure_backend_registered();
  reset_state();

  const connection conn(make_connection_info());
  state().fail_execute = true;

  REQUIRE(conn.begin().is_error());
  REQUIRE(conn.commit().is_error());
  REQUIRE(conn.rollback().is_error());
  REQUIRE(state().execute_count == 3);
}

TEST_CASE("connection metadata helpers delegate to backend", "[connection]") {
  ensure_backend_registered();
  reset_state();

  const connection conn(make_connection_info());

  REQUIRE(conn.describe("person").is_ok());
  REQUIRE(state().describe_count == 1);
  REQUIRE(state().last_describe_table == "person");

  const auto explicit_exists = conn.exists("public", "person");
  REQUIRE(explicit_exists.is_ok());
  REQUIRE(*explicit_exists);
  REQUIRE(state().last_exists_schema == "public");
  REQUIRE(state().last_exists_table == "person");

  const auto default_schema_exists = conn.exists("address");
  REQUIRE(default_schema_exists.is_ok());
  REQUIRE(*default_schema_exists);
  REQUIRE(state().last_exists_schema == "default_schema");
  REQUIRE(state().last_exists_table == "address");

  const auto sequence_exists = conn.sequence_exists("person_id_seq");
  REQUIRE(sequence_exists.is_ok());
  REQUIRE(*sequence_exists);
  REQUIRE(state().last_sequence_schema == "default_schema");
  REQUIRE(state().last_sequence_name == "person_id_seq");
}

TEST_CASE("connection returns injected resolver service", "[connection]") {
  ensure_backend_registered();
  reset_state();

  auto resolver = std::make_shared<resolver_service>();
  const connection conn(make_connection_info(), resolver);

  REQUIRE(conn.resolver() == resolver);
}

TEST_CASE("connection replaces null resolver service with default resolver", "[connection]") {
  ensure_backend_registered();
  reset_state();

  const connection conn(make_connection_info(), std::shared_ptr<resolver_service>{});

  REQUIRE(conn.resolver() != nullptr);
}

TEST_CASE("connection tolerates explicitly passed null logger", "[connection]") {
  ensure_backend_registered();
  reset_state();

  const connection conn(make_connection_info(), connection::logger_ptr{});

  REQUIRE(conn.open().is_ok());
  REQUIRE(conn.execute(make_context("UPDATE person SET name = 'Jane'")).is_ok());
  REQUIRE(conn.fetch(make_context("SELECT * FROM person")).is_ok());
  REQUIRE(conn.prepare(make_context("SELECT * FROM person WHERE id = ?")).is_ok());
}

TEST_CASE("connection move construction transfers ownership", "[connection]") {
  ensure_backend_registered();
  reset_state();

  {
    connection source(make_connection_info());
    REQUIRE(state().create_count == 1);

    connection target(std::move(source));
    REQUIRE(target.type() == test_backend_type);
    REQUIRE(state().destroy_count == 0);
  }

  REQUIRE(state().destroy_count == 1);
}

TEST_CASE("connection move assignment destroys previous backend and transfers ownership", "[connection]") {
  ensure_backend_registered();
  reset_state();

  {
    connection first(make_connection_info());
    connection second(make_connection_info());

    REQUIRE(state().create_count == 2);
    REQUIRE(state().destroy_count == 0);

    second = std::move(first);

    REQUIRE(state().destroy_count == 1);
    REQUIRE(second.type() == test_backend_type);
  }

  REQUIRE(state().destroy_count == 2);
}

TEST_CASE("connection str returns sql from query context", "[connection]") {
  ensure_backend_registered();
  reset_state();

  const connection conn(make_connection_info());

  REQUIRE(conn.str(make_context("SELECT 1")) == "SELECT 1");
}