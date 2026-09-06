#ifndef QUERY_QUERY_PARTS_HPP
#define QUERY_QUERY_PARTS_HPP

#include "matador/query/criteria/abstract_criteria.hpp"
#include "matador/query/internal/column_value_pair.hpp"
#include "matador/query/placeholder.hpp"
#include "matador/query/column.hpp"
#include "matador/query/constraint.hpp"
#include "matador/query/query_context.hpp"
#include "matador/query/query_part.hpp"
#include "matador/query/table.hpp"

#include <list>
#include <memory>

namespace matador::query::internal {

class query_alter_part final : public query_part {
public:
  query_alter_part();

  void accept(query_part_visitor &visitor) override;
};

class query_alter_table_part final : public query_part {
public:
  explicit query_alter_table_part(class table tab);

  void accept(query_part_visitor &visitor) override;

  [[nodiscard]] const class table& table() const;

private:
  class table table_;
};

class query_add_key_constraint_part final : public query_part {
public:
  explicit query_add_key_constraint_part(std::string name);
  void accept(query_part_visitor &visitor) override;

  [[nodiscard]] const std::string &name() const;

private:
  std::string name_;
};

class query_add_constraint_part_by_constraint final : public query_part {
public:
  explicit query_add_constraint_part_by_constraint(const constraint &co);
  void accept(query_part_visitor &visitor) override;

  [[nodiscard]] const class constraint &constraint() const;

private:
  class constraint constraint_;
};

class query_drop_key_constraint_part_by_name final : public query_part {
public:
  explicit query_drop_key_constraint_part_by_name(std::string name);
  void accept(query_part_visitor &visitor) override;

  [[nodiscard]] const std::string &name() const;

private:
  std::string name_;
};

class query_drop_key_constraint_part_by_constraint final : public query_part {
public:
  explicit query_drop_key_constraint_part_by_constraint(const constraint &co);
  void accept(query_part_visitor &visitor) override;

  [[nodiscard]] const class constraint &constraint() const;

private:
  class constraint constraint_;
};

class query_add_foreign_key_constraint_part final : public query_part {
public:
  explicit query_add_foreign_key_constraint_part(const std::vector<column> &columns);
  void accept(query_part_visitor &visitor) override;

  [[nodiscard]] const std::vector<column> &columns() const;

private:
  std::vector<column> columns_;
};

class query_add_foreign_key_reference_part final : public query_part {
public:
  explicit query_add_foreign_key_reference_part(class table tab,
                                                const std::vector<column> &columns);
  void accept(query_part_visitor &visitor) override;

  [[nodiscard]] const class table &table() const;
  [[nodiscard]] const std::vector<column> &columns() const;

private:
  class table table_;
  std::vector<column> columns_;
};

class query_add_primary_key_constraint_part final : public query_part {
public:
  explicit query_add_primary_key_constraint_part(const std::vector<column> &columns);
  void accept(query_part_visitor &visitor) override;

  [[nodiscard]] const std::vector<column> &columns() const;

private:
  std::vector<column> columns_;
};

/**
 * Represents the SQL SELECT part
 */
class query_select_part final : public query_part {
public:
  explicit query_select_part(std::vector<column> columns);
  void accept(query_part_visitor &visitor) override;

  [[nodiscard]] const std::vector<column> &columns() const;

private:
  std::vector<column> columns_;
};

class query_select_nextval_part final : public query_part {
public:
  explicit query_select_nextval_part(std::string sequence_name);

  [[nodiscard]] const std::string &sequence_name() const;

  void accept(query_part_visitor &visitor) override;

private:
  std::string sequence_name_;
};

class query_select_currval_part final : public query_part {
public:
  explicit query_select_currval_part(std::string sequence_name);

  [[nodiscard]] const std::string &sequence_name() const;

  void accept(query_part_visitor &visitor) override;

private:
  std::string sequence_name_;
};

/**
 * Represents the SQL FROM part
 */
class query_from_part final : public query_part {
public:
  explicit query_from_part(std::vector<table> tables);

  [[nodiscard]] const std::vector <table> &tables() const;

  void accept(query_part_visitor &visitor) override;

private:
  std::vector<table> tables_;
};

class query_join_table_part final : public query_part {
public:
  explicit query_join_table_part(class table tab);

  [[nodiscard]] const class table &table() const;

  void accept(query_part_visitor &visitor) override;

private:
  class table table_;
};

class query_join_query_part final : public query_part {
public:
  explicit query_join_query_part(query_context  ctx);

  [[nodiscard]] const query_context& query() const;

  void accept(query_part_visitor &visitor) override;

private:
  query_context ctx_;
};

class query_on_part final : public query_part {
public:
  template <class Condition>
  explicit query_on_part(const Condition &cond)
  : query_part(dialect_token::On), condition_(new Condition(cond)) {}
  explicit query_on_part(std::unique_ptr<abstract_criteria> &&cond);

  [[nodiscard]] const abstract_criteria &condition() const;

  void accept(query_part_visitor &visitor) override;

private:
  std::unique_ptr<abstract_criteria> condition_;
};

class query_where_part final : public query_part {
public:
  template <class Condition>
  explicit query_where_part(const Condition &cond)
  : query_part(dialect_token::Where), condition_(new Condition(cond)) {}
  explicit query_where_part(std::unique_ptr<abstract_criteria> &&cond);

  [[nodiscard]] const abstract_criteria &condition() const;

  void accept(query_part_visitor &visitor) override;

private:
  std::unique_ptr<abstract_criteria> condition_;
};

class table_name_part : public query_part {
protected:
  explicit table_name_part(dialect_token token, std::string table_name);

protected:
  std::string table_name_;
};

class query_group_by_part final : public query_part {
public:
  explicit query_group_by_part(const std::vector<column> &columns);

  [[nodiscard]] const std::vector<column> &columns() const;

  void accept(query_part_visitor &visitor) override;

private:
  std::vector<column> columns_;
};

class query_order_by_part final : public query_part {
public:
  explicit query_order_by_part(const std::vector<column> &columns);

  [[nodiscard]] const std::vector<column> &columns() const;

  void accept(query_part_visitor &visitor) override;

private:
  std::vector<column> columns_;
};

class query_order_by_asc_part final : public query_part {
public:
  query_order_by_asc_part();

  void accept(query_part_visitor &visitor) override;
};

class query_order_by_desc_part final : public query_part {
public:
  query_order_by_desc_part();

  void accept(query_part_visitor &visitor) override;
};

class query_offset_part final : public query_part {
public:
  explicit query_offset_part(size_t offset);

  [[nodiscard]] size_t offset() const;

  void accept(query_part_visitor &visitor) override;

private:
  size_t offset_;
};

class query_limit_part final : public query_part {
public:
  explicit query_limit_part(size_t limit);

  [[nodiscard]] size_t limit() const;

  void accept(query_part_visitor &visitor) override;

private:
  size_t limit_;
};

class query_insert_part final : public query_part {
public:
  query_insert_part();

  void accept(query_part_visitor &visitor) override;
};

class query_into_part final : public query_part {
public:
  query_into_part(class table tab, std::vector<column> columns);

  [[nodiscard]] const class table &table() const;
  [[nodiscard]] const std::vector<column> &columns() const;

  void accept(query_part_visitor &visitor) override;

private:
  class table table_;
  std::vector<column> columns_;
};
/**
 * Represents the SQL VALUES part
 */
class query_values_part final : public query_part {
public:
  explicit query_values_part(std::vector<std::variant<placeholder, utils::database_type>> &&values);

  [[nodiscard]] const std::vector<std::variant<placeholder, utils::database_type>> &values() const;

  void accept(query_part_visitor &visitor) override;

private:
  std::vector<std::variant<placeholder, utils::database_type>> values_;
};

class query_returning_part final : public query_part {
public:
  explicit query_returning_part(std::vector<column> columns);

  [[nodiscard]] const std::vector<column> &columns() const;

  void accept(query_part_visitor &visitor) override;

private:
  std::vector<column> columns_;
};

class query_update_part final : public query_part {
public:
  explicit query_update_part(class table tab);

  [[nodiscard]] const class table &table() const;

  void accept(query_part_visitor &visitor) override;

private:
  class table table_;
};

class query_set_part final : public query_part {
public:
  explicit query_set_part(std::vector<column_value_pair> &&key_value_pairs);

  [[nodiscard]] const std::vector<column_value_pair> &column_values() const;

  void accept(query_part_visitor &visitor) override;

private:
  std::vector<column_value_pair> key_value_pairs_;
};

class query_delete_part final : public query_part {
public:
  query_delete_part();

  void accept(query_part_visitor &visitor) override;
};

class query_delete_from_part final : public query_part {
public:
  explicit query_delete_from_part(class table tab);

  [[nodiscard]] const class table &table() const;

  void accept(query_part_visitor &visitor) override;

private:
  class table table_;
};

class query_create_part final : public query_part {
public:
  query_create_part();

  void accept(query_part_visitor &visitor) override;
};

class query_create_table_part final : public query_part {
public:
  explicit query_create_table_part(class table tab);

  [[nodiscard]] const class table &table() const;

  void accept(query_part_visitor &visitor) override;

private:
  class table table_;
};

class query_create_table_columns_part final : public query_part {
public:
  explicit query_create_table_columns_part(const std::list<column> &columns);

  [[nodiscard]] const std::list<column> &columns() const;

  void accept(query_part_visitor &visitor) override;

private:
  std::list<column> columns_;
};

class query_create_table_constraints_part final : public query_part {
public:
  explicit query_create_table_constraints_part(const std::list<constraint> &constraints);

  [[nodiscard]] const std::list<constraint> &constraints() const;

  void accept(query_part_visitor &visitor) override;

private:
  std::list<constraint> constraints_;
};

class query_create_sequence_part final : public query_part {
public:
  explicit query_create_sequence_part(std::string sequence_name);

  [[nodiscard]] const std::string &sequence_name() const;

  void accept(query_part_visitor &visitor) override;

private:
  std::string sequence_name_;
};

class query_create_schema_part final : public query_part {
public:
  explicit query_create_schema_part(std::string schema);

  [[nodiscard]] const std::string &schema() const;

  void accept(query_part_visitor &visitor) override;

private:
  std::string schema_;
};

class query_drop_part final : public query_part {
public:
  query_drop_part();

  void accept(query_part_visitor &visitor) override;
};

class query_drop_table_part final : public query_part {
public:
  explicit query_drop_table_part(class table tab);

  [[nodiscard]] const class table &table() const;

  void accept(query_part_visitor &visitor) override;

private:
  class table table_;
};

class query_drop_sequence_part final : public query_part {
public:
  explicit query_drop_sequence_part(std::string sequence_name);

  [[nodiscard]] const std::string &sequence_name() const;

  void accept(query_part_visitor &visitor) override;

private:
  std::string sequence_name_;
};

class query_drop_schema_part final : public query_part {
public:
  explicit query_drop_schema_part(std::string schema_);

  [[nodiscard]] const std::string &schema() const;

  void accept(query_part_visitor &visitor) override;

private:
  std::string schema_;
};

} // namespace matador::query::internal
#endif // QUERY_QUERY_PARTS_HPP
