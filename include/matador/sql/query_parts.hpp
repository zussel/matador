#ifndef QUERY_QUERY_PARTS_HPP
#define QUERY_QUERY_PARTS_HPP

#include "matador/sql/basic_condition.hpp"
#include "matador/sql/query_part_visitor.hpp"
#include "matador/sql/column.hpp"
#include "matador/sql/column_definition.hpp"
#include "matador/sql/key_value_pair.hpp"
#include "matador/sql/query_part.hpp"
#include "matador/sql/table.hpp"

#include <memory>

namespace matador::sql {

class basic_condition;

/**
 * Represents the SQL SELECT part
 */
class query_select_part : public query_part
{
public:
  explicit query_select_part(std::vector<column> columns);
  void accept(query_part_visitor &visitor) override;

  [[nodiscard]] const std::vector<column>& columns() const;

private:
  std::vector<column> columns_;
};

/**
 * Represents the SQL FROM part
 */
class query_from_part : public query_part
{
public:
  explicit query_from_part(sql::table t);

  [[nodiscard]] const sql::table& table() const;

private:
  void accept(query_part_visitor &visitor) override;

private:
  sql::table table_;
};

class query_join_part : public query_part
{
public:
  explicit query_join_part(sql::table t);

  [[nodiscard]] const sql::table& table() const;

private:
  void accept(query_part_visitor &visitor) override;

private:
  sql::table table_;
};

class query_on_part : public query_part
{
public:
  template < class Condition >
  explicit query_on_part(const Condition &cond)
    : query_part(dialect_token::ON)
    , condition_(new Condition(cond)) {}
  explicit query_on_part(std::unique_ptr<basic_condition> &&cond);

  [[nodiscard]] const basic_condition& condition() const;

private:
  void accept(query_part_visitor &visitor) override;

private:
  std::unique_ptr<basic_condition> condition_;
};

class query_where_part : public query_part
{
public:
  template < class Condition >
  explicit query_where_part(const Condition &cond)
  : query_part(dialect_token::WHERE)
  , condition_(new Condition(cond)) {}
  explicit query_where_part(std::unique_ptr<basic_condition> &&cond);

  [[nodiscard]] const basic_condition& condition() const;

private:
  void accept(query_part_visitor &visitor) override;

private:
  std::unique_ptr<basic_condition> condition_;
};

class query_table_name_part : public query_part
{
protected:
  explicit query_table_name_part(sql::dialect_token token, std::string table_name);

protected:
  std::string table_name_;
};

class query_group_by_part : public query_part
{
public:
  explicit query_group_by_part(sql::column  col);

  [[nodiscard]] const sql::column& column() const;

private:
  void accept(query_part_visitor &visitor) override;

private:
  sql::column column_;
};

class query_order_by_part : public query_part
{
public:
  explicit query_order_by_part(sql::column  col);

  [[nodiscard]] const sql::column& column() const;

private:
  void accept(query_part_visitor &visitor) override;

private:
  sql::column column_;
};

class query_order_by_asc_part : public query_part
{
public:
  query_order_by_asc_part();

private:
  void accept(query_part_visitor &visitor) override;
};

class query_order_by_desc_part : public query_part
{
public:
  query_order_by_desc_part();

private:
  void accept(query_part_visitor &visitor) override;
};

class query_offset_part : public query_part
{
public:
  explicit query_offset_part(size_t offset);

  [[nodiscard]] size_t offset() const;

private:
  void accept(query_part_visitor &visitor) override;

private:
  size_t offset_;
};

class query_limit_part : public query_part
{
public:
  explicit query_limit_part(size_t limit);

  [[nodiscard]] size_t limit() const;

private:
  void accept(query_part_visitor &visitor) override;

private:
  size_t limit_;
};

class query_insert_part : public query_part
{
public:
  query_insert_part();

private:
  void accept(query_part_visitor &visitor) override;
};

class query_into_part : public query_part
{
public:
  query_into_part(sql::table t, std::vector<sql::column> columns);

  [[nodiscard]] const sql::table& table() const;
  [[nodiscard]] const std::vector<column>& columns() const;

private:
  void accept(query_part_visitor &visitor) override;

private:
  sql::table table_;
  std::vector<column> columns_;
};
/**
 * Represents the SQL VALUES part
 */
class query_values_part : public query_part
{
public:
  explicit query_values_part(std::vector<utils::any_type> &&values);

  [[nodiscard]] const std::vector<utils::any_type>& values() const;

private:
  void accept(query_part_visitor &visitor) override;

private:
  std::vector<utils::any_type> values_;
};

class query_update_part : public query_part
{
public:
  explicit query_update_part(sql::table table);

  [[nodiscard]] const sql::table& table() const;

private:
  void accept(query_part_visitor &visitor) override;

private:
  sql::table table_;
};

class query_set_part : public query_part
{
public:
  explicit query_set_part(const std::vector<sql::key_value_pair>& key_value_pairs);

  [[nodiscard]] const std::vector<sql::key_value_pair>& key_values() const;

private:
  void accept(query_part_visitor &visitor) override;

private:
  std::vector<sql::key_value_pair> key_value_pairs_;
};

class query_delete_part : public query_part
{
public:
  query_delete_part();

private:
  void accept(query_part_visitor &visitor) override;
};

class query_delete_from_part : public query_part
{
public:
  query_delete_from_part(sql::table table);

  [[nodiscard]] const sql::table& table() const;

private:
  void accept(query_part_visitor &visitor) override;

private:
  sql::table table_;
};

class query_create_part : public query_part
{
public:
  query_create_part();

private:
  void accept(query_part_visitor &visitor) override;
};

class query_create_table_part : public query_part
{
public:
  query_create_table_part(sql::table table, std::vector<sql::column_definition> columns);

  [[nodiscard]] const sql::table& table() const;
  [[nodiscard]] const std::vector<sql::column_definition>& columns() const;

private:
  void accept(query_part_visitor &visitor) override;

private:
  sql::table table_;
  std::vector<sql::column_definition> columns_;
};

class query_drop_part : public query_part
{
public:
  query_drop_part();

private:
  void accept(query_part_visitor &visitor) override;
};

class query_drop_table_part : public query_part
{
public:
  explicit query_drop_table_part(sql::table table);

  [[nodiscard]] const sql::table& table() const;

private:
  void accept(query_part_visitor &visitor) override;

private:
  sql::table table_;
};

}
#endif //QUERY_QUERY_PARTS_HPP
