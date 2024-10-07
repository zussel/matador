#include "matador/sql/query_compiler.hpp"
#include "matador/sql/query_compile_context.hpp"
#include "matador/sql/column_definition.hpp"
#include "matador/sql/connection.hpp"
#include "matador/sql/dialect.hpp"
#include "matador/sql/any_type_to_string_visitor.hpp"

#include "matador/utils/string.hpp"

namespace matador::sql {

query_context query_compiler::compile(const query_compile_context &data, const connection_impl &conn)
{
  data_ = &data;
  connection_ = &conn;
  query_ = {};
  for (const auto &part: data.parts) {
    part->accept(*this);
  }
  connection_ = nullptr;
  data_ = nullptr;

  return query_;
}

std::string handle_column(query_context &ctx, const dialect &d, const query_compile_context &compiler_ctx, const column &col) {
    ctx.result_vars.emplace_back(col.name);
    const auto& column_table = col.table_.get();
    ctx.column_aliases.insert({column_table->has_alias() ? column_table->alias : column_table->name + "." + col.name, col.alias});
    if (col.is_function()) {
      ctx.prototype.emplace_back(col.has_alias() ? col.alias : col.name);
      ctx.prototype.back().type(data_type::type_long);
    } else {
        ctx.prototype.emplace_back(col.name);
    }

    if (const auto it = compiler_ctx.tables.find(col.table_->name); it != compiler_ctx.tables.end()) {
        return d.prepare_identifier({it->second, col.name, col.alias});
    }

    return d.prepare_identifier(col);
}

void query_compiler::visit(query_select_part &select_part)
{
  query_.sql = connection_->dialect().token_at(dialect_token::SELECT) + " ";

  query_.prototype.clear();

  std::string result;
  if (const auto &columns = select_part.columns(); columns.size() < 2) {
    for (const auto &col: columns) {
      result.append(handle_column(query_, connection_->dialect(), *data_, col ));
    }
  } else {
    auto it = columns.begin();
    result.append(handle_column(query_, connection_->dialect(), *data_, *it++));
    for (; it != columns.end(); ++it) {
      result.append(", ");
      result.append(handle_column(query_, connection_->dialect(), *data_, *it));
    }
  }

  query_.sql += result;
}

void query_compiler::visit(query_from_part &from_part)
{
  query_.table = from_part.table();
  query_.sql += " " + query_compiler::build_table_name(from_part.token(), connection_->dialect(), query_.table);
  query_.table_aliases.insert({query_.table.name, query_.table.alias});
}

void query_compiler::visit(query_join_part &join_part)
{
  query_.sql += " " + query_compiler::build_table_name(join_part.token(), connection_->dialect(), join_part.table());
}

void query_compiler::visit(query_on_part &on_part)
{
  query_.sql += " " + connection_->dialect().token_at(dialect_token::ON) +
                " " + on_part.condition().evaluate(connection_->dialect(), query_);
}

void query_compiler::visit(query_where_part &where_part)
{
  query_.sql += " " + connection_->dialect().token_at(dialect_token::WHERE) +
                " " + where_part.condition().evaluate(connection_->dialect(), query_);
}

void query_compiler::visit(query_group_by_part &group_by_part)
{
  query_.sql += " " + connection_->dialect().token_at(dialect_token::GROUP_BY) + " " + connection_->dialect().prepare_identifier(group_by_part.column());
}

void query_compiler::visit(query_order_by_part &order_by_part)
{
  query_.sql += " " + connection_->dialect().token_at(dialect_token::ORDER_BY) +
                " " + connection_->dialect().prepare_condition(order_by_part.column());
}

void query_compiler::visit(query_order_by_asc_part &/*order_by_asc_part*/)
{
  query_.sql += " " + connection_->dialect().token_at(dialect_token::ASC);
}

void query_compiler::visit(query_order_by_desc_part &/*order_by_desc_part*/)
{
  query_.sql += " " + connection_->dialect().token_at(dialect_token::DESC);
}

void query_compiler::visit(query_offset_part &offset_part)
{
  query_.sql += " " + connection_->dialect().token_at(dialect_token::OFFSET) + " " + std::to_string(offset_part.offset());
}

void query_compiler::visit(query_limit_part &limit_part)
{
  query_.sql += " " + connection_->dialect().token_at(dialect_token::LIMIT) + " " + std::to_string(limit_part.limit());
}

void query_compiler::visit(query_insert_part &/*insert_part*/)
{
  query_.sql = connection_->dialect().token_at(dialect_token::INSERT);
}

void query_compiler::visit(query_into_part &into_part)
{
  query_.table = into_part.table();
  query_.sql += " " + connection_->dialect().token_at(dialect_token::INTO) +
                " " + connection_->dialect().prepare_identifier_string(into_part.table().name);

  std::string result{"("};
  if (into_part.columns().size() < 2) {
    for (const auto &col: into_part.columns()) {
      result.append(connection_->dialect().prepare_identifier_string(col.name));
    }
  } else {
    auto it = into_part.columns().begin();
    result.append(connection_->dialect().prepare_identifier_string((it++)->name));
    for (; it != into_part.columns().end(); ++it) {
      result.append(", ");
      result.append(connection_->dialect().prepare_identifier_string(it->name));
    }
  }
  result += (")");
  query_.sql += " " + result;
}

void query_compiler::visit(query_values_part &values_part)
{
  query_.sql += " " + connection_->dialect().token_at(dialect_token::VALUES);

  attribute_string_writer writer(*connection_);
  any_type_to_string_visitor value_to_string(writer, query_);

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
  query_.sql += query_compiler::build_table_name(update_part.token(), connection_->dialect(), query_.table);
}

void query_compiler::visit(query_delete_part &/*delete_part*/)
{
  query_.sql = connection_->dialect().token_at(dialect_token::REMOVE);
}

void query_compiler::visit(query_delete_from_part &delete_from_part)
{
  query_.table = delete_from_part.table();
  query_.sql += " " + query_compiler::build_table_name(delete_from_part.token(), connection_->dialect(), query_.table);
}

void query_compiler::visit(query_create_part &/*create_part*/)
{
  query_.sql = connection_->dialect().token_at(dialect_token::CREATE);
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
  query_.sql += " " + connection_->dialect().token_at(dialect_token::TABLE) + " " + connection_->dialect().prepare_identifier_string(create_table_part.table().name) + " ";
  query_.table = create_table_part.table();

  std::string result = "(";

  column_context context;

  if (create_table_part.columns().size() < 2) {
    for (const auto &col: create_table_part.columns()) {
      result.append(build_create_column(col, connection_->dialect(), context));
    }
  } else {
    auto it = create_table_part.columns().begin();
    result.append(build_create_column(*it++, connection_->dialect(), context));
    for (; it != create_table_part.columns().end(); ++it) {
      result.append(", ");
      result.append(build_create_column(*it, connection_->dialect(), context));
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
  query_.sql = connection_->dialect().token_at(dialect_token::DROP);
}

void query_compiler::visit(query_set_part &set_part)
{
  query_.sql += " " + connection_->dialect().token_at(dialect_token::SET) + " ";

  attribute_string_writer writer(*connection_);
  any_type_to_string_visitor value_to_string(writer, query_);
  std::string result;
  if (set_part.key_values().size() < 2) {
    for (const auto &col: set_part.key_values()) {
      result.append(connection_->dialect().prepare_identifier_string(col.name()) + "=");
      auto var = col.value();
      std::visit(value_to_string, var);
      result.append(value_to_string.result);
    }
  } else {
    auto it = set_part.key_values().begin();
    result.append(connection_->dialect().prepare_identifier_string(it->name()) + "=");
    auto var = (it++)->value();
    std::visit(value_to_string, var);
    result.append(value_to_string.result);
    for (; it != set_part.key_values().end(); ++it) {
      result.append(", ");
      result.append(connection_->dialect().prepare_identifier_string((*it).name()) + "=");
      var = it->value();
      std::visit(value_to_string, var);
      result.append(value_to_string.result);
    }
  }

  query_.sql += result;
}

void query_compiler::visit(query_drop_table_part &drop_table_part)
{
  query_.table = drop_table_part.table();
  query_.sql += " " + query_compiler::build_table_name(drop_table_part.token(), connection_->dialect(), query_.table);
}

std::string build_create_column(const column_definition &col, const dialect &d, column_context &context)
{
  std::string result = d.prepare_identifier_string(col.name()) + " " + d.data_type_at(col.type());
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

std::string query_compiler::build_table_name(const dialect_token token, const dialect &d, const table& t)
{
  return d.token_at(token) + " " +
    (!d.default_schema_name().empty() ? d.prepare_identifier_string(d.default_schema_name()) + "." : "") +
    d.prepare_identifier_string(t.name) +
    (t.alias.empty() ? "" : " " + d.prepare_identifier_string(t.alias));
}

}