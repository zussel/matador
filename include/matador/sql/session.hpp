#ifndef QUERY_SESSION_HPP
#define QUERY_SESSION_HPP

#include "matador/sql/connection.hpp"
#include "matador/sql/connection_pool.hpp"
#include "matador/sql/entity_query_builder.hpp"
#include "matador/sql/statement.hpp"
#include "matador/sql/schema.hpp"

#include "matador/object/object_ptr.hpp"

#include <unordered_map>

namespace matador::sql {

class dialect;

enum class session_error {
  Ok = 0,
  NoConnectionAvailable,
  UnknownType,
  FailedToBuildQuery,
  FailedToFindObject
};

/**
 * @brief Represents a session to a database
 *
 * This class represents a session to a database
 * identified by a persistence object.
 *
 * @code
 *
 * matador::sql::schema p(<database connection string>);
 * p.attach<person>("person");
 *
 * matador::session s(p);
 *
 * @endcode
 *
 * Once a session object is instantiated one can
 * insert, update and delete objects.
 *
 * @code
 *
 * s.insert(new person("georg"));
 *
 * @endcode
 *
 * This session can also load the whole database into
 * the underlying object_store.
 */
class session
{
public:
  explicit session(connection_pool<connection> &pool);

  template<typename Type>
  void attach(const std::string &table_name) const;

  void create_schema() const;

  template<typename Type>
  object_ptr<Type> insert(Type *obj);

  template< class Type, typename... Args >
  object_ptr<Type> insert(Args&&... args) {
    return insert(new Type(std::forward<Args>(args)...));
  }

  template<typename Type, typename PrimaryKeyType>
  utils::result<object_ptr<Type>, session_error> find(const PrimaryKeyType &pk) {
    auto c = pool_.acquire();
    if (!c.valid()) {
      return utils::error(session_error::NoConnectionAvailable);
    }
    auto info = schema_->info<Type>();
    if (!info) {
      return utils::error(session_error::UnknownType);
    }

    entity_query_builder eqb(*schema_);
    auto data = eqb.build<Type>(pk);
    if (!data.is_ok()) {
      return utils::error(session_error::FailedToBuildQuery);
    }

    auto obj = build_select_query(c, data.release()).template fetch_one<Type>();

    if (!obj) {
      return utils::error(session_error::FailedToFindObject);
    }
    return utils::ok(object_ptr<Type>{ obj.release() });
  }

  template<typename Type>
  utils::result<query_result<Type>, session_error> find() {
    auto c = pool_.acquire();
    if (!c.valid()) {
      return utils::error(session_error::NoConnectionAvailable);
    }
    auto info = schema_->info<Type>();
    if (!info) {
      return utils::error(session_error::UnknownType);
    }

    entity_query_builder eqb(*schema_);
    auto data = eqb.build<Type>();
    if (!data.is_ok()) {
      return utils::error(session_error::FailedToBuildQuery);
    }

    return utils::ok(build_select_query(c, data.release()).template fetch_all<Type>());
  }

  template<typename Type>
  utils::result<query_from_intermediate, session_error> select() {
    auto c = pool_.acquire();
    if (!c.valid()) {
      return utils::error(session_error::NoConnectionAvailable);
    }
    auto info = schema_->info<Type>();
    if (!info) {
      return utils::error(session_error::UnknownType);
    }

    entity_query_builder eqb(*schema_);
    auto data = eqb.build<Type>();
    if (!data.is_ok()) {
      return utils::error(session_error::FailedToBuildQuery);
    }

    return utils::ok(build_select_query(c, data.release()).template fetch_all<Type>());
  }

  template<typename Type>
  void drop_table();
  void drop_table(const std::string &table_name) const;

//  [[nodiscard]] query_result<record> fetch(const query_context &q) const;
//  [[nodiscard]] query_result<record> fetch(const std::string &sql) const;
  [[nodiscard]] size_t execute(const std::string &sql) const;
  statement prepare(query_context q) const;

  std::vector<sql::column_definition> describe_table(const std::string &table_name) const;
  bool table_exists(const std::string &table_name) const;

  const class dialect& dialect() const;

private:
  friend class fetchable_query;

  [[nodiscard]] std::unique_ptr<query_result_impl> fetch(const std::string &sql) const;

  fetchable_query build_select_query(connection_ptr<connection> &conn, entity_query_data &&data) const;

private:
  connection_pool<connection> &pool_;
  const class dialect &dialect_;

  std::unique_ptr<schema> schema_;
  mutable std::unordered_map<std::string, table_definition> prototypes_;
};

template<typename Type>
void session::attach(const std::string &table_name) const
{
  schema_->attach<Type>(table_name);
}

template<typename Type>
object_ptr<Type> session::insert(Type *obj)
{
  auto c = pool_.acquire();
  auto info = schema_->info<Type>();
  if (!info) {
    return {};
  }
  c->query(*schema_)
    .insert()
    .into(info->name, column_generator::generate<Type>(*schema_, true))
    .values(*obj)
    .execute();

  return object_ptr<Type>{obj};
}

template<typename Type>
void session::drop_table()
{
  auto info = schema_->info<Type>();
  if (info) {
    return drop_table(info.name);
  }

}
}
#endif //QUERY_SESSION_HPP
