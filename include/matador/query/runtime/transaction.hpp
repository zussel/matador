#ifndef MATADOR_QUERY_TRANSACTION_HPP
#define MATADOR_QUERY_TRANSACTION_HPP

#include "matador/query/internal/execute_step.hpp"

#include "matador/utils/error.hpp"
#include "matador/utils/result.hpp"

#include <memory>
#include <vector>

namespace matador::query {

class session;

class transaction final {
public:
  struct transaction_context {
    bool active_{true};

    std::vector<std::unique_ptr<execute_step>> insert_steps_;
    std::vector<std::unique_ptr<execute_step>> update_steps_;
    std::vector<std::unique_ptr<execute_step>> remove_steps_;

    void add_insert_steps(std::vector<std::unique_ptr<execute_step>> steps);
    void add_update_steps(std::vector<std::unique_ptr<execute_step>> steps);
    void add_remove_steps(std::vector<std::unique_ptr<execute_step>> steps);
  };

public:
  explicit transaction(session &ses, const std::shared_ptr<transaction_context>& context);

  transaction(const transaction&) = delete;
  transaction& operator=(const transaction&) = delete;
  transaction(transaction&&) noexcept = default;
  transaction& operator=(transaction&&) noexcept = delete;
  ~transaction();

  [[nodiscard]] bool active() const;

  result<void, error> commit() const;
  result<void, error> rollback() const;

private:
  result<void, error> execute_steps(executor &conn, std::vector<std::unique_ptr<execute_step>> &steps) const;
  result<void, error> finalize_steps(const std::vector<std::unique_ptr<execute_step>> &steps) const;

private:
  session &session_;
  std::shared_ptr<transaction_context> context_;
};

}

#endif // MATADOR_QUERY_TRANSACTION_HPP