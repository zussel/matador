#include "matador/sql/query_compiler.hpp"
#include "matador/sql/query_data.hpp"
#include "matador/sql/column_definition.hpp"
#include "matador/sql/dialect.hpp"
#include "matador/sql/any_type_to_string_visitor.hpp"

#include "matador/utils/string.hpp"

namespace matador::sql {

query_compiler::query_compiler(const sql::dialect &d)
: dialect_(d)
{}

query_context query_compiler::compile(const query_data *data)
{
  for (const auto &part: data->parts) {
    part->accept(*this);
  }

  return query_;
}

void append_column(std::vector<column_definition> &columns, const column &col)
{
    if (col.is_function()) {
        columns.emplace_back(col.has_alias() ? col.alias : col.name);
    } else {
        columns.emplace_back(col.name);
    }
}

void query_compiler::visit(query_select_part &select_part)
{
  query_.sql = dialect_.token_at(dialect::token_t::SELECT) + " ";

  query_.prototype.clear();

  std::string result;
  const auto &columns = select_part.columns();
  if (columns.size() < 2) {
    for (const auto &col: columns) {
      result.append(dialect_.prepare_identifier(col));
      query_.result_vars.emplace_back(col.name);
      append_column(query_.prototype, col);
    }
  } else {
    auto it = columns.begin();
    result.append(dialect_.prepare_identifier(*it));
    query_.result_vars.emplace_back(it->name);
    append_column(query_.prototype, *it++);
    for (; it != columns.end(); ++it) {
      result.append(", ");
      result.append(dialect_.prepare_identifier(*it));
      query_.result_vars.emplace_back(it->name);
      append_column(query_.prototype, *it);
    }
  }

  query_.sql += result;
}

void query_compiler::visit(query_from_part &from_part)
{
  query_.table = from_part.table();
  if (dialect_.default_schema_name().empty()) {
    query_.sql += " " + dialect_.token_at(dialect::token_t::FROM) +
                  " " + dialect_.prepare_identifier(from_part.table().name) +
                  (from_part.table().alias.empty() ? "" : " AS " +
                                                          dialect_.prepare_identifier(from_part.table().alias));
  } else {
    query_.sql += " " + dialect_.token_at(dialect::token_t::FROM) +
                  " " + dialect_.prepare_identifier(dialect_.default_schema_name()) +
                  "." + dialect_.prepare_identifier(from_part.table().name) +
                  (from_part.table().alias.empty() ? "" : " AS " +
                                                          dialect_.prepare_identifier(from_part.table().alias));
  }
}

void query_compiler::visit(query_join_part &join_part)
{
  if (dialect_.default_schema_name().empty()) {
    query_.sql += " " + dialect_.token_at(dialect::token_t::JOIN) +
                  " " + dialect_.prepare_identifier(join_part.table().name) +
                  (join_part.table().alias.empty() ? "" : " AS " + dialect_.prepare_identifier(join_part.table().alias));
  } else {
    query_.sql += " " + dialect_.token_at(dialect::token_t::JOIN) +
                  " " + dialect_.prepare_identifier(dialect_.default_schema_name()) +
                  "." + dialect_.prepare_identifier(join_part.table().name) +
                  (join_part.table().alias.empty() ? "" : " AS " + dialect_.prepare_identifier(join_part.table().alias));

  }
}

void query_compiler::visit(query_on_part &on_part)
{
  query_.sql += " " + dialect_.token_at(dialect::token_t::ON) +
                " " + on_part.condition().evaluate(const_cast<dialect &>(dialect_), query_);
}

void query_compiler::visit(query_where_part &where_part)
{
  query_.sql += " " + dialect_.token_at(dialect::token_t::WHERE) +
                " " + where_part.condition().evaluate(const_cast<dialect &>(dialect_), query_);
}

void query_compiler::visit(query_group_by_part &group_by_part)
{
  query_.sql += " " + dialect_.token_at(dialect::token_t::GROUP_BY) + " " + dialect_.prepare_identifier(group_by_part.column());
}

void query_compiler::visit(query_order_by_part &order_by_part)
{
  query_.sql += " " + dialect_.token_at(dialect::token_t::ORDER_BY) +
                " " + dialect_.prepare_identifier(order_by_part.column());
}

void query_compiler::visit(query_order_by_asc_part &/*order_by_asc_part*/)
{
  query_.sql += " " + dialect_.token_at(dialect::token_t::ASC);
}

void query_compiler::visit(query_order_by_desc_part &/*order_by_desc_part*/)
{
  query_.sql += " " + dialect_.token_at(dialect::token_t::DESC);
}

void query_compiler::visit(query_offset_part &offset_part)
{
  query_.sql += " " + dialect_.token_at(dialect::token_t::OFFSET) + " " + std::to_string(offset_part.offset());
}

void query_compiler::visit(query_limit_part &limit_part)
{
  query_.sql += " " + dialect_.token_at(dialect::token_t::LIMIT) + " " + std::to_string(limit_part.limit());
}

void query_compiler::visit(query_insert_part &/*insert_part*/)
{
  query_.sql = dialect_.token_at(dialect::token_t::INSERT);
}

void query_compiler::visit(query_into_part &into_part)
{
  query_.table = into_part.table();
  query_.sql += " " + dialect_.token_at(dialect::token_t::INTO) +
                " " + dialect_.prepare_identifier(into_part.table().name);

  std::string result{"("};
  if (into_part.columns().size() < 2) {
    for (const auto &col: into_part.columns()) {
      result.append(dialect_.prepare_identifier(col.name));
    }
  } else {
    auto it = into_part.columns().begin();
    result.append(dialect_.prepare_identifier((it++)->name));
    for (; it != into_part.columns().end(); ++it) {
      result.append(", ");
      result.append(dialect_.prepare_identifier(it->name));
    }
  }
  result += (")");
  query_.sql += " " + result;
}

void query_compiler::visit(query_values_part &values_part)
{
  query_.sql += " " + dialect_.token_at(dialect::token_t::VALUES);

  any_type_to_string_visitor value_to_string(dialect_, query_);

  std::string result{"("};
  if (values_part.values().size() < 2) {
    for (auto val: values_part.values()) {
      std::visit(value_to_string, val);
      result.append(value_to_string.result);
    }
  } else {
    auto it = values_part.values().begin();
    auto val = *it++;
    std::visit(value_to_string, val);
    result.append(value_to_string.result);
    for (; it != values_part.values().end(); ++it) {
      result.append(", ");
      val = *it;
      std::visit(value_to_string, val);
      result.append(value_to_string.result);
    }
  }
  result += (")");

  query_.sql += " " + result;
}

void query_compiler::visit(query_update_part &update_part)
{
  query_.table = update_part.table();
  query_.sql = dialect_.token_at(dialect::token_t::UPDATE) + " " + dialect_.prepare_identifier(update_part.table().name);
}

void query_compiler::visit(query_delete_part &/*delete_part*/)
{
  query_.sql = dialect_.token_at(dialect::token_t::REMOVE);
}

void query_compiler::visit(query_delete_from_part &delete_from_part)
{
  query_.table = delete_from_part.table();
  query_.sql += " " + dialect_.token_at(delete_from_part.token()) +
                " " + dialect_.prepare_identifier(delete_from_part.table().name);
}

void query_compiler::visit(query_create_part &/*create_part*/)
{
  query_.sql = dialect_.token_at(dialect::token_t::CREATE);
}

struct fk_context
{
  std::string column;
  std::string ref_table;
  std::string ref_column;
};

struct column_context
{
  std::vector<std::string> primary_keys;
  std::vector<fk_context> foreign_contexts;
};

std::string build_create_column(const column_definition &col, const dialect &d, column_context &context);

void query_compiler::visit(query_create_table_part &create_table_part)
{
  query_.sql += " " + dialect_.token_at(dialect::token_t::TABLE) + " " + dialect_.prepare_identifier(create_table_part.table().name) + " ";
  query_.table = create_table_part.table();

  std::string result = "(";

  column_context context;

  if (create_table_part.columns().size() < 2) {
    for (const auto &col: create_table_part.columns()) {
      result.append(build_create_column(col, dialect_, context));
    }
  } else {
    auto it = create_table_part.columns().begin();
    result.append(build_create_column(*it++, dialect_, context));
    for (; it != create_table_part.columns().end(); ++it) {
      result.append(", ");
      result.append(build_create_column(*it, dialect_, context));
    }
  }

  if (!context.primary_keys.empty()) {
    result.append(", CONSTRAINT PK_" + create_table_part.table().name + " PRIMARY KEY (" + utils::join(context.primary_keys, ", ") + ")");
  }
  for (const auto &fk: context.foreign_contexts) {
    result += ", CONSTRAINT FK_" + create_table_part.table().name;
    result += "_" + fk.column;
    result += " FOREIGN KEY (" + fk.column + ")";
    result += " REFERENCES " + fk.ref_table + "(" + fk.ref_column + ")";
  }

  result += ")";
  query_.sql += result;
}

void query_compiler::visit(query_drop_part &/*drop_part*/)
{
  query_.sql = dialect_.token_at(dialect::token_t::DROP);
}

void query_compiler::visit(query_set_part &set_part)
{
  query_.sql += " " + dialect_.token_at(dialect::token_t::SET) + " ";

  any_type_to_string_visitor value_to_string(dialect_, query_);
  std::string result;
  if (set_part.key_values().size() < 2) {
    for (const auto &col: set_part.key_values()) {
      result.append(dialect_.prepare_identifier(col.name()) + "=");
      auto var = col.value();
      std::visit(value_to_string, var);
      result.append(value_to_string.result);
    }
  } else {
    auto it = set_part.key_values().begin();
    result.append(dialect_.prepare_identifier(it->name()) + "=");
    auto var = (it++)->value();
    std::visit(value_to_string, var);
    result.append(value_to_string.result);
    for (; it != set_part.key_values().end(); ++it) {
      result.append(", ");
      result.append(dialect_.prepare_identifier((*it).name()) + "=");
      var = it->value();
      std::visit(value_to_string, var);
      result.append(value_to_string.result);
    }
  }

  query_.sql += result;
}

void query_compiler::visit(query_drop_table_part &drop_table_part)
{
  query_.sql += " " + dialect_.token_at(dialect::token_t::TABLE) + " " + dialect_.prepare_identifier(drop_table_part.table().name);
  query_.table = drop_table_part.table();
}

std::string build_create_column(const column_definition &col, const dialect &d, column_context &context)
{
  std::string result = d.prepare_identifier(col.name()) + " " + d.data_type_at(col.type());
  if (col.attributes().size() > 0) {
    result.append("(" + std::to_string(col.attributes().size()) + ")");
  }
  if (!col.is_nullable()) {
    result.append(" NOT NULL");
  }
  if (is_constraint_set(col.attributes().options(), utils::constraints::UNIQUE)) {
    result.append(" UNIQUE");
  }
  if (is_constraint_set(col.attributes().options(), utils::constraints::PRIMARY_KEY)) {
    context.primary_keys.emplace_back(col.name());
  }
  if (is_constraint_set(col.attributes().options(), utils::constraints::FOREIGN_KEY)) {
    context.foreign_contexts.push_back({col.name(), col.ref_table(), col.ref_column()});
  }

  return result;
}

}