#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#include "matador/sql/statement_impl.hpp"
#include "matador/sql/result.hpp"
#include "matador/sql/basic_sql_logger.hpp"

#include <string>
#include <functional>
#include <utility>

namespace matador {

class sql;

namespace detail {
  class statement_impl;
  template < class Type >
  class identifier_binder;
}

/**
 * The statement class represents a prepared
 * sql statement for a concrete object type.
 * 
 * @tparam T The object type of the statement to work on
 */
template < class T = row >
class statement
{
public:
  statement(const statement &x) = delete;
  statement& operator=(const statement &x) = delete;

public:
  /**
   * Creates an empty statement
   */
  statement() = default;

  /**
   * Creates a statement initialized from the
   * given statement implementation object holding
   * the implementation for the selected database
   *
   * @param impl The statement implementation object
   * @param sqllogger The logger handler to write sql log messages to
   */
  statement(detail::statement_impl *impl, std::shared_ptr<basic_sql_logger> sqllogger)
    : statement(impl, T{}, sqllogger)
  {}

  /**
   * Creates a statement initialized from the
   * given statement implementation object holding
   * the implementation for the selected database
   *
   * @param impl The statement implementation object
   * @param prototype Row object containing prototype columns
   * @param sqllogger The logger handler to write sql log messages to
   */
  statement(detail::statement_impl *impl, T prototype, std::shared_ptr<basic_sql_logger> sqllogger)
  : p(impl)
  , prototype_(std::move(prototype))
  , logger_(std::move(sqllogger))
  { }


  ~statement() = default;

  /**
   * Copy move constructor for statement
   * 
   * @param x The statement to move from
   */
  statement(statement &&x) noexcept
  : p(std::move(x.p))
  , prototype_(x.prototype_)
  , logger_(std::move(x.logger_))
  {}

  /**
   * Assignment move constructor for statement
   * 
   * @param x The statement to move from
   * @return Reference to this
   */
  statement& operator=(statement &&x) noexcept
  {
    p = std::move(x.p);
    prototype_ = std::move(x.prototype_);
    logger_ = std::move(x.logger_);
    return *this;
  }

  /**
   * Clear the statement
   */
  void clear()
  {
    if (p) {
      p->clear();
    }
  }

  /**
   * Executes the prepared statement and returns
   * a result object. If the sql command was a
   * select the result object holds the queried
   * rows.
   * 
   * @return The result of the statement
   */
  result<T> execute()
  {
    logger_->on_execute(p->str());
    return { p->execute(), prototype_ };
  }

  /**
   * Resets the statement by unbinding
   * all bindings.
   */
  void reset()
  {
    p->reset();
  }
  
  /**
   * Bind an object to the statement starting
   * at the given position index.
   * 
   * @param index The index where to start the binding
   * @param obj The object to bind
   * @return The next index to bind
   */
  std::size_t bind(std::size_t index, T *obj)
  {
    return p->bind(obj, index);
  }

  /**
   * Bind single value to a specified
   * position index of the prepared statement
   * 
   * @tparam The type of the value
   * @param val The value to bind
   * @param index The index where the value is to bind
   * @return The next index to bind
   */
  std::size_t bind(std::size_t index, std::string &val, size_t size = 0)
  {
    return p->bind(val, size, index);
  }

  template < class V >
  std::size_t bind(std::size_t index, V &val)
  {
    return p->bind(val, index);
  }

  /**
   * Returns the statement as string where
   * the host values a shown as question marks (?)
   * 
   * @return The query string
   */
  std::string str() const
  {
    return p->str();
  }

  /**
   * Enable console log of sql statements
   */
  void enable_log()
  {
    p->enable_log();
  }

  /**
   * Disable console log of sql statements
   */
  void disable_log()
  {
    p->disable_log();
  }

  /**
   * Returns true if logging is enabled.
   *
   * @return True if logging is enabled
   */
  bool is_log_enabled() const
  {
    return p->is_log_enabled();
  }

private:
  template < class Type >
  friend class detail::identifier_binder;

private:
  std::unique_ptr<matador::detail::statement_impl> p;
  T prototype_{};
  std::shared_ptr<basic_sql_logger> logger_;
};

}

#endif /* STATEMENT_HPP */
