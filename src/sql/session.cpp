#include "matador/sql/session.hpp"

#include "matador/sql/backend_provider.hpp"
#include "matador/sql/query.hpp"

#include <stdexcept>

namespace matador::sql {

session::session(connection_pool<connection> &pool)
: pool_(pool)
, dialect_(backend_provider::instance().connection_dialect(pool_.info().type))
, schema_(std::make_unique<sql::schema>(dialect_.default_schema_name())){}

utils::result<void, sql_error> session::create_schema() const {
  auto c = pool_.acquire();

  return schema_->create(*c);
}

void session::drop_table(const std::string &table_name) const {
  auto c = pool_.acquire();
  if (!c.valid()) {
    throw std::logic_error("no database connection available");
  }

  const auto res = query::drop()
    .table(table_name)
    .execute(*c);
}

//query_result<record> session::fetch(const query_context &q) const
//{
//  auto c = pool_.acquire();
//  if (!c.valid()) {
//    throw std::logic_error("no database connection available");
//  }
//  auto it = prototypes_.find(q.table.name);
//  if (it == prototypes_.end()) {
//    it = prototypes_.emplace(q.table.name, c->describe(q.table.name)).first;
//  }
//  // adjust columns from given query
//  for (auto &col : q.prototype) {
//    if (const auto rit = it->second.find(col.name()); col.type() == data_type::type_unknown && rit != it->second.end()) {
//      const_cast<column_definition&>(col).type(rit->type());
//    }
//  }
//  auto res = c->fetch(q.sql);
//  return query_result<record>{std::move(res), q.prototype};
//}

//query_result<record> session::fetch(const std::string &sql) const
//{
//  return query_result<record>(std::unique_ptr());
//}

size_t session::execute(const std::string &sql) const {
  auto c = pool_.acquire();
  if (!c.valid()) {
    throw std::logic_error("no database connection available");
  }
  return c->execute(sql);
}

// statement session::prepare(query_context q) const
// {
//   auto c = pool_.acquire();
//   if (!c.valid()) {
//     throw std::logic_error("no database connection available");
//   }
//   return c->prepare(std::move(q));
// }

utils::result<std::vector<sql::column_definition>, session_error> session::describe_table(const std::string &table_name) const
{
  auto c = pool_.acquire();
  if (!c.valid()) {
    throw std::logic_error("no database connection available");
  }
  return c->describe(table_name).or_else([](const auto &err) {
    return utils::error(session_error{session_error_code::FailedToDescribe, "", err});
  });
}

bool session::table_exists(const std::string &table_name) const
{
  auto c = pool_.acquire();
  if (!c.valid()) {
    throw std::logic_error("no database connection available");
  }
  return c->exists(dialect_.default_schema_name(), table_name);
}

const class dialect &session::dialect() const
{
  return dialect_;
}

std::unique_ptr<query_result_impl> session::fetch(const std::string &sql) const
{
  auto c = pool_.acquire();
  if (!c.valid()) {
    throw std::logic_error("no database connection available");
  }
  // return c->fetch(sql);
    return {};
}

fetchable_query session::build_select_query(entity_query_data &&data) {
  return query::select(data.columns)
    .from(*data.root_table)
    .join_left(data.joins)
    .where(std::move(data.where_clause))
    .order_by(column{*data.pk_column_})
    .asc();
}

}