#include "matador/query/query_builder.hpp"

#include "matador/query/internal/attribute_string_writer.hpp"
#include "matador/query/expression_evaluator.hpp"
#include "matador/query/query_data.hpp"
#include "matador/query/criteria_evaluator.hpp"
#include "matador/query/query_utils.hpp"
#include "matador/query/constraint.hpp"
#include "matador/query/column.hpp"

#include "matador/query/internal/basic_type_to_string_visitor.hpp"
#include "matador/query/internal/string_builder_utils.hpp"
#include "matador/query/internal/query_parts.hpp"

#include "matador/query/query_context.hpp"
#include "matador/query/dialect.hpp"

namespace matador::query {
query_context query_builder::build(const query_data& data,
                                        const dialect& d,
                                        const std::optional<std::reference_wrapper<const connection_impl>> conn) {
  data_ = &data;
  dialect_ = &d;
  connection_ = conn;
  query_ = {};
  for (const auto& part : data.parts) {
    part->accept(*this);
  }
  finisher_(query_);
  connection_ = std::nullopt;
  dialect_ = nullptr;
  data_ = nullptr;

  query_.sql_hash = std::hash<std::string>{}(query_.sql);
  return {query_};
}

static void build_columns_with_name_only(std::string& out, const std::vector<column>& cols, const dialect& d);
static void build_columns(std::string& out, const std::vector<column>& cols, const dialect& d);
static void build_fetchable_columns(query_context& ctx, const std::vector<column>& cols, const dialect& d);
static void prepare_prototype(std::vector<column>& prototype, const column& col);
static void prepare_prototype(std::vector<column>& prototype, const std::string& column_name);

void query_builder::visit(internal::query_alter_part& part) {
  query_.sql = dialect_->token_at(part.token());
}

void query_builder::visit(internal::query_alter_table_part& part) {
  query_.command = query_command::AlterTable;
  query_.sql += " " + dialect_->token_at(part.token()) + " " +
    prepare_identifier_string(part.table().name(), *dialect_);
}

void query_builder::visit(internal::query_add_key_constraint_part& part) {
  query_.sql += " " + dialect_->add_constraint() + " " + part.name();
}

void query_builder::visit(internal::query_add_foreign_key_constraint_part& part) {
  query_.sql += " " + dialect_->token_at(part.token()) + " (";
  build_columns(query_.sql, part.columns(), *dialect_);
  query_.sql += ')';
}

void query_builder::visit(internal::query_add_primary_key_constraint_part& part) {
  query_.sql += " " + dialect_->primary_key() + " (";
  build_columns(query_.sql, part.columns(), *dialect_);
  query_.sql += ')';
}

void query_builder::visit(internal::query_add_foreign_key_reference_part& part) {
  query_.sql += " " + dialect_->token_at(part.token()) + " " + part.table().name() + " (";
  build_columns(query_.sql, part.columns(), *dialect_);
  query_.sql += ')';
}

void query_builder::visit(internal::query_add_constraint_part_by_constraint& part) {
  query_.sql += build_add_constraint_string(part.constraint());
}

void query_builder::visit(internal::query_drop_key_constraint_part_by_name& part) {
  query_.sql += " " + dialect_->token_at(part.token()) + " " + part.name();
}

void query_builder::visit(internal::query_drop_key_constraint_part_by_constraint& part) {
  query_.sql += " " + build_drop_constraint_string(part.constraint());
}

void query_builder::visit(internal::query_select_part& part) {
  query_.command = query_command::Select;
  query_.sql = dialect_->select() + " ";

  query_.columns.clear();

  if (part.columns().empty()) {
    query_.sql += dialect_->asterisk();
  } else {
    build_fetchable_columns(query_, part.columns(), *dialect_);
  }
}

void query_builder::visit(internal::query_select_nextval_part& part) {
  query_.sql += dialect_->select() + " " + dialect_->nextval() + "('" + part.sequence_name() + "')";
  prepare_prototype(query_.columns, part.sequence_name());
}

void query_builder::visit(internal::query_select_currval_part& part) {
  query_.sql += dialect_->select() + " " + dialect_->currval() + "('" + part.sequence_name() + "')";
  prepare_prototype(query_.columns, part.sequence_name());
}

void query_builder::visit(internal::query_from_part& part) {
  query_.table_name = part.tables().front().name();
  query_.sql += " " + dialect_->from() + " ";
  if (const auto& tables = part.tables(); tables.size() < 2) {
    for (const auto& tab : tables) {
      query_.sql += build_table_name(*dialect_, tab);
    }
  } else {
    auto it = tables.begin();
    query_.sql.append(build_table_name(*dialect_, *it++));
    for (; it != tables.end(); ++it) {
      query_.sql.append(", ");
      query_.sql.append(build_table_name(*dialect_, *it));
    }
  }
}

void query_builder::visit(internal::query_join_table_part& part) {
  query_.sql += " " + build_table_name(part.token(), *dialect_, part.table());
}

void query_builder::visit(internal::query_join_query_part& part) {
  query_.sql += " " + dialect_->join() + " (" + part.query().sql + ")";
}

void query_builder::visit(internal::query_on_part& part) {
  criteria_evaluator evaluator(*dialect_, query_);
  query_.sql += " " + dialect_->on() +
    " " + evaluator.evaluate(part.condition());
}

void query_builder::visit(internal::query_where_part& part) {
  criteria_evaluator evaluator(*dialect_, query_);
  query_.sql += " " + dialect_->where() +
    " " + evaluator.evaluate(part.condition());
}

void query_builder::visit(internal::query_group_by_part& part) {
  query_.sql += " " + dialect_->group_by() + " ";
  if (part.columns().size() < 2) {
    for (const auto& col : part.columns()) {
      query_.sql.append(prepare_identifier_string(col.name(), *dialect_));
    }
  } else {
    auto it = part.columns().begin();
    query_.sql.append(prepare_identifier_string((it++)->canonical_name(), *dialect_));
    for (; it != part.columns().end(); ++it) {
      query_.sql.append(", ");
      query_.sql.append(prepare_identifier_string(it->canonical_name(), *dialect_));
    }
  }
}

void query_builder::visit(internal::query_order_by_part& part) {
  query_.sql += " " + dialect_->order_by() + " ";
  if (part.columns().size() < 2) {
    for (const auto& col : part.columns()) {
      query_.sql.append(prepare_identifier_string(col.canonical_name(), *dialect_));
    }
  } else {
    auto it = part.columns().begin();
    query_.sql.append(prepare_identifier_string((it++)->canonical_name(), *dialect_));
    for (; it != part.columns().end(); ++it) {
      query_.sql.append(", ");
      query_.sql.append(prepare_identifier_string(it->canonical_name(), *dialect_));
    }
  }
}

void query_builder::visit(internal::query_order_by_asc_part&/*order_by_asc_part*/) {
  query_.sql += " " + dialect_->asc();
}

void query_builder::visit(internal::query_order_by_desc_part&/*order_by_desc_part*/) {
  query_.sql += " " + dialect_->desc();
}

void query_builder::visit(internal::query_offset_part& part) {
  query_.sql += " " + dialect_->offset() + " " + std::to_string(part.offset());
}

void query_builder::visit(internal::query_limit_part& part) {
  query_.sql += " " + dialect_->limit() + " " + std::to_string(part.limit());
}

void query_builder::visit(internal::query_insert_part&/*insert_part*/) {
  query_.command = query_command::Insert;
  query_.sql = dialect_->insert();
}

void query_builder::visit(internal::query_into_part& part) {
  query_.table_name = part.table().name();
  query_.sql += " " + dialect_->into() +
    " " + prepare_identifier_string(part.table().name(), *dialect_) + " (";

  build_columns_with_name_only(query_.sql, part.columns(), *dialect_);
  query_.sql += ')'/* + result*/;
}

struct value_visitor {
  value_visitor(attribute_string_writer& w, query_context& ctx)
  : value_to_string_visitor(w, ctx) {
  }

  void operator()(const utils::database_type& val) {
    std::visit(value_to_string_visitor, val);
  }

  void operator()(const placeholder&/*val*/) {
    value_to_string_visitor.query.bind_vars.emplace_back(
      std::string("value_") + std::to_string(value_to_string_visitor.query.bind_vars.size() + 1));
    value_to_string_visitor.result = value_to_string_visitor.writer->dialect().next_placeholder(
      value_to_string_visitor.query.bind_vars);
  }

  internal::basic_type_to_string_visitor value_to_string_visitor;
};

std::string query_builder::determine_value(const dialect& d, query_context& ctx,
                                           const abstract_column_expression& exp) {
  attribute_string_writer writer(d, {});
  expression_evaluator v(d, ctx);
  exp.accept(v);

  return v.result();
}

std::string query_builder::determine_value(value_visitor& visitor, const std::variant<placeholder, utils::database_type>& val) {
  std::visit(visitor, val);
  return visitor.value_to_string_visitor.result;
}

void query_builder::visit(internal::query_values_part& part) {
  query_.sql += " " + dialect_->values();

  attribute_string_writer writer(*dialect_, connection_);

  value_visitor visitor(writer, query_);
  std::string result{"("};
  if (part.values().size() < 2) {
    for (const auto& val : part.values()) {
      result.append(determine_value(visitor, val));
    }
  } else {
    auto it = part.values().begin();
    auto val = *it++;
    result.append(determine_value(visitor, val));
    for (; it != part.values().end(); ++it) {
      result.append(", ");
      val = *it;
      result.append(determine_value(visitor, val));
    }
  }
  result += (')');

  query_.sql += " " + result;
}

void query_builder::visit(internal::query_returning_part& part) {
  query_.sql += " " + dialect_->returning() + " ";

  build_fetchable_columns(query_, part.columns(), *dialect_);
}

void query_builder::visit(internal::query_update_part& part) {
  query_.command = query_command::Update;
  query_.table_name = part.table().name();
  query_.sql += build_table_name(part.token(), *dialect_, query_.table_name);
}

void query_builder::visit(internal::query_delete_part&/*delete_part*/) {
  query_.command = query_command::Delete;
  query_.sql = dialect_->remove();
}

void query_builder::visit(internal::query_delete_from_part& part) {
  query_.table_name = part.table().name();
  query_.sql += " " + build_table_name(part.token(), *dialect_, query_.table_name);
}

void query_builder::visit(internal::query_create_part&/*create_part*/) {
  query_.command = query_command::CreateTable;
  query_.sql = dialect_->create();
}

void build_create_column(std::string& out, const column& col, const dialect& d);
std::string build_constraint(const constraint& cons, const dialect& d);

void query_builder::visit(internal::query_create_table_part& part) {
  query_.sql += " " + dialect_->table() + " " + prepare_identifier_string(part.table().name(), *dialect_) + " (";
  query_.table_name = part.table().name();

  finisher_ = [](query_context& ctx) { ctx.sql += ')'; };
}

void query_builder::visit(internal::query_create_table_columns_part& part) {
  bool first = true;
  for (const auto& col : part.columns()) {
    if (!first) {
      query_.sql.append(", ");
    }
    build_create_column(query_.sql, col, *dialect_);
    first = false;
  }
}

void query_builder::visit(internal::query_create_table_constraints_part& part) {
  std::string result;
  for (const auto& c : part.constraints()) {
    result.append(", ");
    result.append(build_constraint(c, *dialect_));
  }
  query_.sql += result;
}

void query_builder::visit(internal::query_create_sequence_part& part) {
  query_.command = query_command::CreateSequence;
  query_.sql += " " + dialect_->sequence() + " " + prepare_identifier_string(part.sequence_name(), *dialect_);
}

void query_builder::visit(internal::query_create_schema_part& part) {
  query_.command = query_command::CreateSchema;
  query_.sql += " " + dialect_->schema() + " " + prepare_identifier_string(part.schema(), *dialect_);
}

void query_builder::visit(internal::query_drop_part& part) {
  query_.command = query_command::DropTable;
  query_.sql = dialect_->token_at(part.token());
}

void query_builder::visit(internal::query_drop_schema_part& part) {
  query_.sql += " " + dialect_->drop() + " " +
    dialect_->schema() + " " + prepare_identifier_string(part.schema(), *dialect_);
}

void query_builder::visit(internal::query_set_part& part) {
  query_.sql += " " + dialect_->set() + " ";

  attribute_string_writer writer(*dialect_, connection_);
  // std::string result;

  value_visitor visitor(writer, query_);
  bool first = true;
  for (const auto& column_value : part.column_values()) {
    if (!first) {
      query_.sql.append(", ");
    }
    query_.sql.append(prepare_identifier_string(column_value.col().column_name(), *dialect_) + "=");
    query_.sql.append(determine_value(*dialect_, query_, column_value.expression()));
    first = false;
  }

  // if (part.column_values().size() < 2) {
  //   for (const auto &column_value: part.column_values()) {
  //     result.append(dialect_->prepare_identifier_string(column_value.col().name()) + "=");
  //     result.append(determine_value(visitor, column_value.value()));
  //   }
  // } else {
  //   auto it = part.column_values().begin();
  //   result.append(dialect_->prepare_identifier_string(it->col().name()) + "=");
  //   result.append(determine_value(visitor, (it++)->value()));
  //   for (; it != part.column_values().end(); ++it) {
  //     result.append(", ");
  //     result.append(dialect_->prepare_identifier_string(it->col().name()) + "=");
  //     result.append(determine_value(visitor, it->value()));
  //   }
  // }
  //
  // query_.sql += result;
}

void query_builder::visit(internal::query_drop_sequence_part& part) {
  query_.command = query_command::DropSequence;
  query_.sql += " " + dialect_->sequence() + " " + prepare_identifier_string(part.sequence_name(), *dialect_);
}

void query_builder::visit(internal::query_drop_table_part& part) {
  query_.table_name = part.table().name();
  query_.sql += " " + build_table_name(part.token(), *dialect_, query_.table_name);
}

void build_create_column(std::string& out, const column& col, const dialect& d) {
  prepare_identifier_string_append(out, col.canonical_name(), d);
  out += " " + d.data_type_at(col.type());
  if (col.size() > 0) {
    out.append("(" + std::to_string(col.size()) + ")");
  }
  if (!col.is_nullable()) {
    out.append(" ").append(d.not_null());
  }
  const column_constraints c = col.constraints();
  if (c.has(column_constraint::Unique)) {
    out.append(" ").append(d.unique());
  }
  if (c.has(column_constraint::Identity)) {
    out.append(" ").append(d.identity());
  }
  if (c.has(column_constraint::PrimaryKey)) {
    out.append(" ").append(d.primary_key());
  }
}

void build_columns_with_name_only(std::string& out, const std::vector<column>& cols, const dialect& d) {
  bool first = true;
  for (const auto& col : cols) {
    if (!first) {
      out.append(", ");
    }
    prepare_identifier_string_append(out, col.column_name(), d);
    first = false;
  }
}

void build_columns(std::string& out, const std::vector<column>& cols, const dialect& d) {
  bool first = true;
  for (const auto& col : cols) {
    if (!first) {
      out.append(", ");
    }
    prepare_identifier_string_append(out, col.name(), d);
    first = false;
  }
}

void build_fetchable_columns(query_context& ctx, const std::vector<column>& cols, const dialect& d) {
  bool first = true;
  for (const auto& col : cols) {
    if (!first) {
      ctx.sql.append(", ");
    }
    prepare_prototype(ctx.columns, col);
    prepare_column(ctx, d, col);
    first = false;
  }
}

void prepare_prototype(std::vector<column>& prototype, const column& col) {
  prototype.emplace_back(col);
}

void prepare_prototype(std::vector<column> &prototype, const std::string &column_name) {
    prototype.emplace_back(column::make_plain(column_name));
}

std::string build_constraint(const constraint& cons, const dialect& d) {
  std::string result;
  if (!cons.column().column_name().empty()) {
    result.append(d.constraint()).append(" ").append(cons.column().column_name()).append(" ");
  }
  if (cons.is_primary_key_constraint()) {
    result
      .append(d.primary_key())
      .append(" (")
      .append(cons.column().column_name())
      .append(")");
  } else if (cons.is_foreign_key_constraint()) {
    result
      .append(d.foreign_key())
      .append(" (")
      .append(cons.column().column_name())
      .append(") ")
      .append(d.references()).append(" ")
      .append(cons.ref_table().name())
      .append(" (")
      .append(cons.ref_column().column_name())
      .append(")");
  } else {
    // handle error
  }
  return result;
}

std::string query_builder::build_table_name(const dialect_token token, const dialect &d, const std::string &table_name) {
  return d.token_at(token) + " " +
    (!d.default_schema_name().empty() ? prepare_identifier_string(d.default_schema_name(), d) + "." : "") +
    prepare_identifier_string(table_name, d);
}

std::string query_builder::build_table_name(const dialect_token token, const dialect& d, const table& t) {
  return d.token_at(token) + " " + build_table_name(d, t);
}

std::string query_builder::build_table_name(const dialect& d, const table& t) {
  return (!d.default_schema_name().empty() ? prepare_identifier_string(d.default_schema_name(), d) + "." : "") +
    prepare_identifier_string(t.table_name(), d) +
    (!t.has_alias() ? "" : " " + prepare_identifier_string(t.name(), d));
}

std::string query_builder::build_add_constraint_string(const constraint& c) const {
  std::string result = " " + dialect_->add_constraint() + " " + build_constraint_name(c) + " ";
  if (c.is_primary_key_constraint()) {
    result.append(dialect_->primary_key()).append(" (").append(c.column().column_name()).append(")");
  } else if (c.is_foreign_key_constraint()) {
    result.append(dialect_->foreign_key()).append(" (").append(c.column().column_name()).append(") ").
           append(dialect_->references()).append(" ").append(c.ref_table().name()).append(" (").append(
             c.ref_column().column_name()).append(")");
  } else if (c.is_unique_constraint()) {
    result.append(dialect_->unique()).append(" (").append(c.column().column_name()).append(")");
  }
  return result;
}

std::string query_builder::build_drop_constraint_string(const constraint& c) const {
  return dialect_->drop_constraint() + " " + build_constraint_name(c);
}

std::string query_builder::build_constraint_name(const constraint& c) {
  if (c.is_primary_key_constraint()) {
    return "PK_" + c.table().table_name();
  }
  if (c.is_foreign_key_constraint()) {
    return "FK_" + c.column().column_name() + "_" + c.table().table_name();
  }
  if (c.is_unique_constraint()) {
    return "UK_" + c.column().column_name() + "_" + c.table().table_name();
  }
  return "";
}
}
