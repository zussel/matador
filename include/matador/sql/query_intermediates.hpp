#ifndef QUERY_QUERY_INTERMEDIATES_HPP
#define QUERY_QUERY_INTERMEDIATES_HPP

#include "matador/sql/column_definition.hpp"
#include "matador/sql/column_definition_generator.hpp"
#include "matador/sql/column_generator.hpp"
#include "matador/sql/key_value_generator.hpp"
#include "matador/sql/key_value_pair.hpp"
#include "matador/sql/placeholder_generator.hpp"
#include "matador/sql/placeholder_key_value_generator.hpp"
#include "matador/sql/query_result.hpp"
#include "matador/sql/query_compile_context.hpp"
#include "matador/sql/record.hpp"
#include "matador/sql/statement.hpp"
#include "matador/sql/sql_error.hpp"
#include "matador/sql/value_extractor.hpp"

#include "matador/utils/types.hpp"
#include "matador/utils/result.hpp"

#include <string>

namespace matador::sql {

class basic_condition;
class connection;

class query_executor;

class query_intermediate
{
public:
  query_intermediate();
  query_intermediate(const std::shared_ptr<query_compile_context> &context); // NOLINT(*-explicit-constructor)

protected:
  std::shared_ptr<query_compile_context> context_;
};

class executable_query : public query_intermediate
{
public:
  using query_intermediate::query_intermediate;

  [[nodiscard]] utils::result<size_t, sql_error> execute(const query_executor &executor) const;
  [[nodiscard]] statement prepare(const query_executor &executor) const;
  [[nodiscard]] std::string str(const query_executor &executor) const;
};

class fetchable_query : public query_intermediate
{
protected:
  using query_intermediate::query_intermediate;

public:
  template < class Type >
  utils::result<query_result<Type>, sql_error> fetch_all(query_executor &executor)
  {
    auto result = fetch(executor);
    if (!result.is_ok()) {
      return utils::error(result.err());
    }

    return utils::ok(query_result<Type>(result.release()));
  }
  [[nodiscard]] utils::result<query_result<record>, sql_error> fetch_all(const query_executor &executor) const;

  template < class Type >
  utils::result<std::unique_ptr<Type>, sql_error> fetch_one(query_executor &executor)
  {
    auto result = fetch(executor);
    if (!result.is_ok()) {
      return utils::error(result.err());
    }

    auto objects = query_result<Type>(result.release());
    auto first = objects.begin();
    if (first == objects.end()) {
      return utils::ok(std::unique_ptr<Type>{nullptr});
    }

    return utils::ok(std::unique_ptr<Type>{first.release()});
  }

  [[nodiscard]] utils::result<std::optional<record>, sql_error> fetch_one(const query_executor &executor) const;

  template<typename Type>
  utils::result<std::optional<Type>, sql_error> fetch_value(query_executor &executor)
  {
    const auto result = fetch_one(executor);
    if (!result.is_ok()) {
      return utils::error(result.err());
    }

    if (result->has_value()) {
      return utils::ok(std::optional<Type>{result->value().at(0).as<Type>().value()});
    }
    return utils::ok(std::optional<Type>{std::nullopt});
  }

  [[nodiscard]] statement prepare(const query_executor &executor) const;

  [[nodiscard]] std::string str(const query_executor &executor) const;

private:
  [[nodiscard]] utils::result<std::unique_ptr<query_result_impl>, sql_error> fetch(const query_executor &executor) const;
};

class query_offset_intermediate;

class query_limit_intermediate : public fetchable_query
{
public:
  using fetchable_query::fetchable_query;

  query_offset_intermediate offset(size_t offset);
};

class query_offset_intermediate : public fetchable_query
{
public:
  using fetchable_query::fetchable_query;

  query_limit_intermediate limit(size_t limit);
};

class query_order_direction_intermediate : public fetchable_query
{
public:
  using fetchable_query::fetchable_query;

  query_limit_intermediate limit(size_t limit);
};

class query_order_by_intermediate;

class query_group_by_intermediate : public fetchable_query
{
public:
  using fetchable_query::fetchable_query;

  query_order_by_intermediate order_by(const column &col);
};

class query_order_by_intermediate : public query_intermediate
{
public:
  using query_intermediate::query_intermediate;

  query_order_direction_intermediate asc();
  query_order_direction_intermediate desc();
};

class query_where_intermediate : public fetchable_query
{
public:
  using fetchable_query::fetchable_query;

  query_group_by_intermediate group_by(const column &col);
  query_order_by_intermediate order_by(const column &col);
};

class query_join_intermediate;

struct join_data
{
  std::shared_ptr<table> join_table;
  std::unique_ptr<basic_condition> condition;
};

class query_from_intermediate : public fetchable_query
{
public:
  using fetchable_query::fetchable_query;

  query_join_intermediate join_left(const table &t);
  query_from_intermediate join_left(join_data &data);
  query_from_intermediate join_left(std::vector<join_data> &data_vector);

  template<class Condition>
  query_where_intermediate where(const Condition &cond)
  {
    return where_clause(std::make_unique<Condition>(std::move(cond)));
  }
  query_where_intermediate where(std::unique_ptr<basic_condition> &&cond)
  {
    return where_clause(std::move(cond));
  }
  query_group_by_intermediate group_by(const column &col);
  query_order_by_intermediate order_by(const column &col);

private:
  query_where_intermediate where_clause(std::unique_ptr<basic_condition> &&cond);
};

using query_on_intermediate = query_from_intermediate;

class query_join_intermediate : public query_intermediate
{
public:
  using query_intermediate::query_intermediate;

  template<class Condition>
  query_on_intermediate on(const Condition &cond)
  {
    return on_clause(std::make_unique<Condition>(std::move(cond)));
  }
  query_on_intermediate on(std::unique_ptr<basic_condition> &&cond)
  {
    return on_clause(std::move(cond));
  }

private:
  query_on_intermediate on_clause(std::unique_ptr<basic_condition> &&cond);
};

class query_select_intermediate : public query_intermediate
{
public:
  explicit query_select_intermediate(const std::vector<column>& columns);

  query_from_intermediate from(const table& t);
};

template < class Type >
std::vector<utils::any_type> as_placeholder(const Type &obj)
{
  placeholder_generator generator;
  access::process(generator, obj);

  return generator.placeholder_values;
}

class query_into_intermediate : public query_intermediate
{
public:
  using query_intermediate::query_intermediate;

  executable_query values(std::initializer_list<utils::any_type> values);
  executable_query values(std::vector<utils::any_type> &&values);
  template<class Type>
  executable_query values()
  {
    Type obj;
    return values(std::move(as_placeholder(obj)));
  }
  template<class Type>
  executable_query values(const Type &obj)
  {
    return values(std::move(value_extractor::extract(obj)));
  }
};

class query_create_intermediate : public query_intermediate
{
public:
  query_create_intermediate();

  executable_query table(const sql::table &table, std::initializer_list<column_definition> columns);
  executable_query table(const sql::table &table, const std::vector<column_definition> &columns);
  template<class Type>
  executable_query table(const sql::table &table, const sql::schema &schema)
  {
    return this->table(table, column_definition_generator::generate<Type>(schema));
  }
};

class query_drop_intermediate : query_intermediate
{
public:
  query_drop_intermediate();

  executable_query table(const sql::table &table);
};

class query_insert_intermediate : public query_intermediate
{
public:
  query_insert_intermediate();

  template<class Type>
  query_into_intermediate into(const sql::table &table, const sql::schema &schema) {
    return into(table, column_generator::generate<Type>(schema));
  }
  query_into_intermediate into(const sql::table &table, std::initializer_list<column> columns);
  query_into_intermediate into(const sql::table &table, std::vector<column> &&columns);
  query_into_intermediate into(const sql::table &table, const std::vector<std::string> &column_names);
  query_into_intermediate into(const sql::table &table);
};

class query_execute_where_intermediate : public executable_query
{
public:
  using executable_query::executable_query;

  query_limit_intermediate limit(size_t limit);
  query_order_by_intermediate order_by(const column &col);
};

class query_set_intermediate : public executable_query
{
public:
  using executable_query::executable_query;

  template<class Condition>
  query_execute_where_intermediate where(const Condition &cond)
  {
    return where_clause(std::make_unique<Condition>(std::move(cond)));
  }

private:
  query_execute_where_intermediate where_clause(std::unique_ptr<basic_condition> &&cond);
};

template < class Type >
std::vector<key_value_pair> as_key_value_placeholder(const Type &obj)
{
  placeholder_key_value_generator generator;
  access::process(generator, obj);

  return generator.placeholder_values;
}


class query_update_intermediate : public query_intermediate
{
public:
  explicit query_update_intermediate(const sql::table& table);

  query_set_intermediate set(std::initializer_list<key_value_pair> columns);
  query_set_intermediate set(std::vector<key_value_pair> &&columns);
  template<class Type>
  query_set_intermediate set()
  {
    Type obj;
    return set(std::move(as_key_value_placeholder(obj)));
  }
  template<class Type>
  query_set_intermediate set(const Type &obj)
  {
    return set(key_value_generator::generate(obj));
  }
};

class query_delete_from_intermediate : public executable_query
{
public:
  using executable_query::executable_query;

  template<class Condition>
  query_execute_where_intermediate where(const Condition &cond)
  {
    return where_clause(std::make_unique<Condition>(std::move(cond)));
  }

private:
  query_execute_where_intermediate where_clause(std::unique_ptr<basic_condition> &&cond);
};

class query_delete_intermediate : public query_intermediate
{
public:
  query_delete_intermediate();

  query_delete_from_intermediate from(const sql::table &table);
};

}
#endif //QUERY_QUERY_INTERMEDIATES_HPP
