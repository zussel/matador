#include <catch2/catch_test_macros.hpp>

#include "fixtures/SessionFixture.hpp"

#include "matador/query/interface/connection_impl.hpp"
#include "matador/query/internal/execute_step.hpp"
#include "matador/query/runtime/session.hpp"
#include "matador/query/schema/schema.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace {

using namespace matador;
using namespace matador::query;

enum class step_phase {
  None,
  Prepare,
  Execute,
  Finalize
};

class recording_step final : public execute_step {
public:
  recording_step(std::string name, std::vector<std::string> &events, step_phase failing_phase = step_phase::None)
  : execute_step(make_context(name))
  , name_(std::move(name))
  , events_(events)
  , failing_phase_(failing_phase) {}

  result<void, error> prepare(executor &) override {
    events_.push_back(name_ + ".prepare");
    return result_for(step_phase::Prepare);
  }

  result<void, error> execute(statement &) override {
    events_.push_back(name_ + ".execute");
    return result_for(step_phase::Execute);
  }

  result<void, error> finalize(object_cache &, const resolver_service_ptr &) override {
    ++finalize_count_;
    return result_for(step_phase::Finalize);
  }

  [[nodiscard]] size_t finalize_count() const {
    return finalize_count_;
  }

private:
  static query_context make_context(const std::string &name) {
    query_context ctx;
    ctx.sql = "SELECT '" + name + "'";
    return ctx;
  }

  result<void, error> result_for(step_phase phase) const {
    if (failing_phase_ == phase) {
      return failure(error{error_code::Failure, name_ + " failed"});
    }
    return ok<void>();
  }

  std::string name_;
  std::vector<std::string> &events_;
  step_phase failing_phase_;
  size_t finalize_count_{};
};

std::unique_ptr<execute_step> make_step(const std::string &name,
                                        std::vector<std::string> &events,
                                        step_phase failing_phase = step_phase::None) {
  return std::make_unique<recording_step>(name, events, failing_phase);
}

} // namespace

using namespace matador::test;

TEST_CASE_METHOD(SessionFixture, "Transaction commits insert, update, and remove steps in order", "[query][transaction][commit]") {
  session ses(make_session_context(2), scm);
  auto context = std::make_shared<transaction::transaction_context>();
  std::vector<std::string> events;
  std::vector<std::unique_ptr<execute_step>> insert_steps;
  insert_steps.push_back(make_step("insert", events));
  context->add_insert_steps(std::move(insert_steps));
  std::vector<std::unique_ptr<execute_step>> update_steps;
  update_steps.push_back(make_step("update", events));
  context->add_update_steps(std::move(update_steps));
  std::vector<std::unique_ptr<execute_step>> remove_steps;
  remove_steps.push_back(make_step("remove", events));
  context->add_remove_steps(std::move(remove_steps));

  transaction tx(ses, context);

  REQUIRE(tx.active());
  const auto result = tx.commit();

  REQUIRE(result.is_ok());
  REQUIRE_FALSE(tx.active());
  REQUIRE((events == std::vector<std::string>{
    "insert.prepare", "insert.execute",
    "update.prepare", "update.execute",
    "remove.prepare", "remove.execute"
  }));
  REQUIRE(static_cast<recording_step *>(context->insert_steps_.front().get())->finalize_count() >= 1);
  REQUIRE(static_cast<recording_step *>(context->update_steps_.front().get())->finalize_count() >= 1);
  REQUIRE(static_cast<recording_step *>(context->remove_steps_.front().get())->finalize_count() >= 1);
}

TEST_CASE_METHOD(SessionFixture, "Transaction rollback clears queued steps and is idempotent", "[query][transaction][rollback]") {
  session ses(make_session_context(2), scm);
  auto context = std::make_shared<transaction::transaction_context>();
  std::vector<std::string> events;
  std::vector<std::unique_ptr<execute_step>> insert_steps;
  insert_steps.push_back(make_step("insert", events));
  context->add_insert_steps(std::move(insert_steps));
  std::vector<std::unique_ptr<execute_step>> update_steps;
  update_steps.push_back(make_step("update", events));
  context->add_update_steps(std::move(update_steps));
  std::vector<std::unique_ptr<execute_step>> remove_steps;
  remove_steps.push_back(make_step("remove", events));
  context->add_remove_steps(std::move(remove_steps));

  transaction tx(ses, context);

  REQUIRE(tx.rollback().is_ok());
  REQUIRE_FALSE(tx.active());
  REQUIRE(context->insert_steps_.empty());
  REQUIRE(context->update_steps_.empty());
  REQUIRE(context->remove_steps_.empty());
  REQUIRE(tx.rollback().is_ok());
  REQUIRE(events.empty());
}

TEST_CASE_METHOD(SessionFixture, "Transaction destructor automatically rolls back active transactions", "[query][transaction][rollback]") {
  session ses(make_session_context(2), scm);
  auto context = std::make_shared<transaction::transaction_context>();
  std::vector<std::string> events;
  std::vector<std::unique_ptr<execute_step>> steps;
  steps.push_back(make_step("insert", events));
  context->add_insert_steps(std::move(steps));

  {
    transaction tx(ses, context);
    REQUIRE(tx.active());
  }

  REQUIRE_FALSE(context->active_);
  REQUIRE(context->insert_steps_.empty());
  REQUIRE(events.empty());
}

TEST_CASE_METHOD(SessionFixture, "Transaction rejects commit after rollback", "[query][transaction][inactive]") {
  session ses(make_session_context(2), scm);
  auto tx = ses.begin();

  REQUIRE(tx.rollback().is_ok());
  const auto result = tx.commit();

  REQUIRE_FALSE(result.is_ok());
  REQUIRE(result.err().ec() == error_code::InvalidOperation);
}

TEST_CASE_METHOD(SessionFixture, "Transaction propagates step failures", "[query][transaction][error]") {
  session ses(make_session_context(2), scm);
  auto context = std::make_shared<transaction::transaction_context>();
  std::vector<std::string> events;
  std::vector<std::unique_ptr<execute_step>> steps;
  steps.push_back(make_step("failing", events, step_phase::Execute));
  context->add_insert_steps(std::move(steps));
  transaction tx(ses, context);

  const auto result = tx.commit();

  REQUIRE_FALSE(result.is_ok());
  REQUIRE(result.err().message() == "failing failed");
  REQUIRE(tx.active());
  REQUIRE((events == std::vector<std::string>{"failing.prepare", "failing.execute"}));
}

// TEST_CASE_METHOD(SessionFixture, "Transaction reports when no connection can be acquired", "[query][transaction][error]") {
  // session ses(make_session_context(0), scm);
  // auto tx = ses.begin();

  // const auto result = tx.commit();

  // REQUIRE_FALSE(result.is_ok());
  // REQUIRE(result.err().ec() == error_code::FailedToAcquirePool);
  // REQUIRE(tx.active());
// }

TEST_CASE_METHOD(SessionFixture, "Finishing an older transaction leaves the newer session transaction active", "[query][transaction][session]") {
  session ses(make_session_context(2), scm);
  auto older = ses.begin();
  auto newer = ses.begin();

  REQUIRE(older.rollback().is_ok());
  REQUIRE(newer.active());
  REQUIRE(newer.commit().is_ok());
  REQUIRE_FALSE(newer.active());
}