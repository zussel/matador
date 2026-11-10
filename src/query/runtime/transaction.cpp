#include "matador/query/runtime/transaction.hpp"

#include "matador/query/runtime/error_code.hpp"
#include "matador/query/runtime/session.hpp"

namespace matador::query {

transaction::transaction(session &ses, const std::shared_ptr<transaction_context>& context)
: session_(ses)
, context_(context) {
}

transaction::~transaction() {
  if (context_->active_) {
    std::ignore = rollback();
  }
}

bool transaction::active() const {
  return context_->active_;
}

void transaction::transaction_context::add_insert_steps(std::vector<std::unique_ptr<execute_step>> steps) {
  insert_steps_.insert(
    insert_steps_.end(),
    std::make_move_iterator(steps.begin()),
    std::make_move_iterator(steps.end()));
}

void transaction::transaction_context::add_update_steps(std::vector<std::unique_ptr<execute_step>> steps) {
  update_steps_.insert(
    update_steps_.end(),
    std::make_move_iterator(steps.begin()),
    std::make_move_iterator(steps.end()));
}

void transaction::transaction_context::add_remove_steps(std::vector<std::unique_ptr<execute_step>> steps) {
  remove_steps_.insert(
    remove_steps_.end(),
    std::make_move_iterator(steps.begin()),
    std::make_move_iterator(steps.end()));
}

result<void, error> transaction::commit() const {
  if (!context_->active_) {
    return failure(make_error(error_code::InvalidOperation, "Transaction is not active."));
  }

  auto conn = session_.pool_.acquire();
  if (!conn.valid()) {
    return failure(make_error(
      error_code::FailedToAcquirePool,
      "Failed to acquire connection for transaction."));
  }

  if (auto result = conn->begin(); !result) {
    return failure(result.err());
  }

  if (auto result = execute_steps(*conn, context_->insert_steps_); !result) {
    std::ignore = conn->rollback();
    return failure(result.err());
  }

  if (auto result = execute_steps(*conn, context_->update_steps_); !result) {
    std::ignore = conn->rollback();
    return failure(result.err());
  }

  if (auto result = execute_steps(*conn, context_->remove_steps_); !result) {
    std::ignore = conn->rollback();
    return failure(result.err());
  }

  if (auto result = conn->commit(); !result) {
    std::ignore = conn->rollback();
    return failure(result.err());
  }

  finalize_steps(context_->insert_steps_);
  finalize_steps(context_->update_steps_);
  finalize_steps(context_->remove_steps_);

  context_->active_ = false;
  session_.finish_transaction(context_);

  return ok<void>();}

result<void, error> transaction::rollback() const {
  if (!context_->active_) {
    return ok<void>();
  }

  context_->insert_steps_.clear();
  context_->update_steps_.clear();
  context_->remove_steps_.clear();

  context_->active_ = false;
  session_.finish_transaction(context_);

  return ok<void>();
}

result<void, error> transaction::execute_steps(executor &conn, std::vector<std::unique_ptr<execute_step>> &steps) const {
  for (auto &step : steps) {
    if (const auto result = step->prepare(conn); !result.is_ok()) {
      return failure(result.err());
    }

    auto stmt = session_.cache_.acquire(step->ctx());
    if (!stmt.is_ok()) {
      return failure(stmt.err());
    }

    if (const auto result = step->execute(*stmt); !result.is_ok()) {
      return failure(result.err());
    }
  }

  for (auto &step : steps) {
    if (const auto result = step->finalize(session_.object_cache_, session_.resolver_service_); !result.is_ok()) {
      return failure(result.err());
    }
  }

  return ok<void>();
}

result<void, error> transaction::finalize_steps(const std::vector<std::unique_ptr<execute_step>> &steps) const {
  for (auto &step : steps) {
    if (auto result = step->finalize(
          session_.object_cache_,
          session_.resolver_service_); !result) {
      return failure(result.err());
          }
  }

  return ok<void>();
}
}