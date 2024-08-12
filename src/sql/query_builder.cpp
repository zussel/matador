#include "matador/sql/query_builder.hpp"
#include "matador/sql/column_generator.hpp"
#include "matador/sql/column.hpp"
#include "matador/sql/dialect.hpp"

#include "matador/utils/string.hpp"

#include <stdexcept>

namespace matador::sql {

namespace detail {

any_type_to_string_visitor::any_type_to_string_visitor(const dialect &d, query_context &query)
: d(d), query(query)
{}

void any_type_to_string_visitor::to_string(const char *val)
{
  result = "'" + d.prepare_literal(val) + "'";
}

void any_type_to_string_visitor::to_string(std::string &val)
{
  result = "'" + d.prepare_literal(val) + "'";
}

void any_type_to_string_visitor::to_string(utils::blob &val)
{
  // "This is a binary Data string" as binary data:
  // MySQL:    X'5468697320697320612062616E617279204461746120737472696E67'
  // Postgres: E'\\x5468697320697320612062616E617279204461746120737472696E67'
  // MSSQL:    0x5468697320697320612062616E617279204461746120737472696E67
  // Sqlite:   X'5468697320697320612062616E617279204461746120737472696E67'
  result = d.token_at(dialect::token_t::BEGIN_BINARY_DATA) + utils::to_string(val) + d.token_at(dialect::token_t::END_BINARY_DATA);
}

void any_type_to_string_visitor::to_string(utils::placeholder &/*val*/)
{
  query.bind_vars.emplace_back("unknown");
  result = d.next_placeholder(query.bind_vars);
}

}

// poor mens state machine
// but does the job for query
query_builder::query_state_transition_map query_builder::transitions_{
{state_t::QUERY_INIT,            {state_t::QUERY_CREATE,   state_t::QUERY_DROP,     state_t::QUERY_SELECT, state_t::QUERY_INSERT,   state_t::QUERY_UPDATE,   state_t::QUERY_DELETE}},
{state_t::QUERY_CREATE,          {state_t::QUERY_TABLE_CREATE}},
{state_t::QUERY_DROP,            {state_t::QUERY_TABLE_DROP}},
{state_t::QUERY_SELECT,          {state_t::QUERY_FROM}},
{state_t::QUERY_INSERT,          {state_t::QUERY_INTO}},
{state_t::QUERY_UPDATE,          {state_t::QUERY_SET}},
{state_t::QUERY_DELETE,          {state_t::QUERY_FROM}},
{state_t::QUERY_TABLE_CREATE,    {state_t::QUERY_FINISH}},
{state_t::QUERY_TABLE_DROP,      {state_t::QUERY_FINISH}},
{state_t::QUERY_FROM,            {state_t::QUERY_OFFSET,   state_t::QUERY_LIMIT,    state_t::QUERY_WHERE,  state_t::QUERY_ORDER_BY, state_t::QUERY_GROUP_BY, state_t::QUERY_JOIN, state_t::QUERY_FINISH}},
{state_t::QUERY_SET,             {state_t::QUERY_WHERE,    state_t::QUERY_FINISH}},
{state_t::QUERY_JOIN,            {state_t::QUERY_ON}},
{state_t::QUERY_ON,              {state_t::QUERY_OFFSET,   state_t::QUERY_LIMIT,    state_t::QUERY_WHERE,  state_t::QUERY_ORDER_BY, state_t::QUERY_GROUP_BY, state_t::QUERY_JOIN, state_t::QUERY_FINISH}},
{state_t::QUERY_INTO,            {state_t::QUERY_VALUES}},
{state_t::QUERY_WHERE,           {state_t::QUERY_ORDER_BY, state_t::QUERY_GROUP_BY, state_t::QUERY_OFFSET, state_t::QUERY_LIMIT,    state_t::QUERY_FINISH}},
{state_t::QUERY_ORDER_BY,        {state_t::QUERY_ORDER_DIRECTION}},
{state_t::QUERY_ORDER_DIRECTION, {state_t::QUERY_OFFSET,   state_t::QUERY_LIMIT,    state_t::QUERY_FINISH}},
{state_t::QUERY_GROUP_BY,        {state_t::QUERY_ORDER_BY, state_t::QUERY_FINISH}},
{state_t::QUERY_OFFSET,          {state_t::QUERY_LIMIT}},
{state_t::QUERY_LIMIT,           {state_t::QUERY_FINISH}},
{state_t::QUERY_VALUES,          {state_t::QUERY_FINISH}},
{state_t::QUERY_FINISH,          {}},
};

query_builder::query_state_to_string_map query_builder::state_strings_{
{state_t::QUERY_INIT,         "init"},
{state_t::QUERY_CREATE,       "create"},
{state_t::QUERY_DROP,         "drop"},
{state_t::QUERY_SELECT,       "select"},
{state_t::QUERY_INSERT,       "insert"},
{state_t::QUERY_UPDATE,       "update"},
{state_t::QUERY_DELETE,       "delete"},
{state_t::QUERY_TABLE_CREATE, "table"},
{state_t::QUERY_TABLE_DROP,   "table"},
{state_t::QUERY_FROM,         "from"},
{state_t::QUERY_SET,          "set"},
{state_t::QUERY_INTO,         "into"},
{state_t::QUERY_WHERE,        "where"},
{state_t::QUERY_ORDER_BY,     "order_by"},
{state_t::QUERY_GROUP_BY,     "group_by"},
{state_t::QUERY_OFFSET,       "offset"},
{state_t::QUERY_LIMIT,        "limit"},
{state_t::QUERY_FINISH,       "finish"},
};

query_builder::query_command_to_string_map query_builder::command_strings_{
{command_t::UNKNOWN, "unknown"},
{command_t::CREATE,  "create"},
{command_t::DROP,    "drop"},
{command_t::SELECT,  "select"},
{command_t::INSERT,  "insert"},
{command_t::UPDATE,  "update"},
{command_t::REMOVE,  "remove"},
};

query_builder::query_builder(const dialect &d)
: dialect_(d), value_to_string_(d, query_)
{}

query_builder &query_builder::create()
{
  initialize(command_t::CREATE, state_t::QUERY_CREATE);

  query_parts_.emplace_back(dialect::token_t::CREATE, dialect_.token_at(dialect::token_t::CREATE));

  return *this;
}

query_builder &query_builder::drop()
{
  initialize(command_t::DROP, state_t::QUERY_DROP);

  query_parts_.emplace_back(dialect::token_t::DROP, dialect_.token_at(dialect::token_t::DROP));

  return *this;
}

query_builder &query_builder::select(std::initializer_list<column> columns)
{
  return select(std::vector<column>{columns});
}

query_builder &query_builder::select(const std::vector<column> &columns)
{
  initialize(command_t::SELECT, state_t::QUERY_SELECT);

  query_parts_.emplace_back(dialect::token_t::SELECT, dialect_.token_at(dialect::token_t::SELECT) + " ");

  query_.prototype.clear();

  std::string result;
  if (columns.size() < 2) {
    for (const auto &col: columns) {
      result.append(dialect_.prepare_identifier(col));
      query_.result_vars.emplace_back(col.name);
      query_.prototype.emplace_back(col.name);
    }
  } else {
    auto it = columns.begin();
    result.append(dialect_.prepare_identifier(*it));
    query_.result_vars.emplace_back(it->name);
    query_.prototype.emplace_back((*it++).name);
    for (; it != columns.end(); ++it) {
      result.append(", ");
      result.append(dialect_.prepare_identifier(*it));
      query_.result_vars.emplace_back(it->name);
      query_.prototype.emplace_back((*it).name);
    }
  }

  query_parts_.emplace_back(dialect::token_t::COLUMNS, result);
  return *this;
}

query_builder &query_builder::insert()
{
  initialize(command_t::INSERT, state_t::QUERY_INSERT);

  query_parts_.emplace_back(dialect::token_t::INSERT, dialect_.token_at(dialect::token_t::INSERT));

  return *this;
}

query_builder &query_builder::update(const std::string &table)
{
  initialize(command_t::UPDATE, state_t::QUERY_UPDATE);

  query_.table = {table};
  query_parts_.emplace_back(dialect::token_t::UPDATE, dialect_.token_at(dialect::token_t::UPDATE) + " " + dialect_.prepare_identifier(table));

  return *this;
}

query_builder &query_builder::remove()
{
  initialize(command_t::REMOVE, state_t::QUERY_DELETE);

  query_parts_.emplace_back(dialect::token_t::REMOVE, dialect_.token_at(dialect::token_t::REMOVE));

  return *this;
}

query_builder &query_builder::table(const std::string &table, std::initializer_list<column_definition> columns)
{
  return this->table(table, std::vector<column_definition>{columns});
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

query_builder &query_builder::table(const std::string &table, const std::vector<column_definition> &/*columns*/)
{
  transition_to(state_t::QUERY_TABLE_CREATE);

  query_parts_.emplace_back(dialect::token_t::TABLE, " " + dialect_.token_at(dialect::token_t::TABLE) + " " + dialect_.prepare_identifier(table) + " ");
  query_.table = {table};

  std::string result = "(";

  column_context context;

//  if (columns.size() < 2) {
//    for (const auto &col: columns) {
//      result.append(build_create_column(col, dialect_, context));
//    }
//  } else {
//    auto it = columns.begin();
//    result.append(build_create_column(*it++, dialect_, context));
//    for (; it != columns.end(); ++it) {
//      result.append(", ");
//      result.append(build_create_column(*it, dialect_, context));
//    }
//  }

  if (!context.primary_keys.empty()) {
    result.append(", CONSTRAINT PK_" + table + " PRIMARY KEY (" + utils::join(context.primary_keys, ", ") + ")");
  }
  for (const auto &fk: context.foreign_contexts) {
    result += ", CONSTRAINT FK_" + table;
    result += "_" + fk.column;
    result += " FOREIGN KEY (" + fk.column + ")";
    result += " REFERENCES " + fk.ref_table + "(" + fk.ref_column + ")";
  }

  result += ")";
  query_parts_.emplace_back(dialect::token_t::COLUMNS, result);
  return *this;
}

query_builder &query_builder::table(const std::string &table)
{
  transition_to(state_t::QUERY_TABLE_DROP);

  query_parts_.emplace_back(dialect::token_t::TABLE, " " + dialect_.token_at(dialect::token_t::TABLE) + " " + dialect_.prepare_identifier(table));
  query_.table = {table};

  return *this;
}

query_builder &query_builder::into(const std::string &table, std::initializer_list<column> column_names)
{
  return into(table, std::vector<column>{column_names});
}

query_builder &query_builder::into(const std::string &table, const std::vector<column> &column_names)
{
  transition_to(state_t::QUERY_INTO);

  query_parts_.emplace_back(dialect::token_t::INTO, " " + dialect_.token_at(dialect::token_t::INTO) + " " + dialect_.prepare_identifier(table) + " ");
  query_.table = {table};

  std::string result{"("};
  if (column_names.size() < 2) {
    for (const auto &col: column_names) {
      result.append(dialect_.prepare_identifier(col.name));
    }
  } else {
    auto it = column_names.begin();
    result.append(dialect_.prepare_identifier((it++)->name));
    for (; it != column_names.end(); ++it) {
      result.append(", ");
      result.append(dialect_.prepare_identifier(it->name));
    }
  }
  result += (")");

  query_parts_.emplace_back(dialect::token_t::COLUMNS, result);
  return *this;
}

query_builder &query_builder::values(std::initializer_list<utils::any_type> values)
{
  return this->values(std::vector<utils::any_type>{values});
}

query_builder &query_builder::values(const std::vector<utils::any_type> &values)
{
  transition_to(state_t::QUERY_VALUES);

  query_parts_.emplace_back(dialect::token_t::VALUES, " " + dialect_.token_at(dialect::token_t::VALUES) + " ");

  std::string result{"("};
  if (values.size() < 2) {
    for (auto val: values) {
      std::visit(value_to_string_, val);
      result.append(value_to_string_.result);
    }
  } else {
    auto it = values.begin();
    auto val = *it++;
    std::visit(value_to_string_, val);
    result.append(value_to_string_.result);
    for (; it != values.end(); ++it) {
      result.append(", ");
      val = *it;
      std::visit(value_to_string_, val);
      result.append(value_to_string_.result);
    }
  }
  result += (")");

  query_parts_.emplace_back(dialect::token_t::INSERT_VALUES, result);
  return *this;
}

query_builder &query_builder::from(const std::string &table, const std::string &as)
{
  transition_to(state_t::QUERY_FROM);

  if (dialect_.default_schema_name().empty()) {
    query_parts_.emplace_back(dialect::token_t::FROM, " " + dialect_.token_at(dialect::token_t::FROM) +
                                                      " " + dialect_.prepare_identifier(table) +
                                                      (as.empty() ? "" : " AS " + dialect_.prepare_identifier(as)));
  } else {
    query_parts_.emplace_back(dialect::token_t::FROM, " " + dialect_.token_at(dialect::token_t::FROM) +
                                                      " " + dialect_.prepare_identifier(dialect_.default_schema_name()) +
                                                      "." + dialect_.prepare_identifier(table) +
                                                      (as.empty() ? "" : " AS " + dialect_.prepare_identifier(as)));
  }
  query_.table = {table};

  return *this;
}

query_builder &query_builder::join(const std::string &table, join_type_t, const std::string &as)
{
  transition_to(state_t::QUERY_JOIN);

  query_parts_.emplace_back(dialect::token_t::JOIN, " " + dialect_.token_at(dialect::token_t::JOIN) +
                                                    " " + dialect_.prepare_identifier(table) +
                                                    (as.empty() ? "" : " AS " + dialect_.prepare_identifier(as)));

  return *this;
}

query_builder &query_builder::on(const std::string &column, const std::string &join_column)
{
  transition_to(state_t::QUERY_ON);

  query_parts_.emplace_back(dialect::token_t::ON, " " + dialect_.token_at(dialect::token_t::ON) +
                                                  " " + dialect_.prepare_identifier(column) +
                                                  "=" + dialect_.prepare_identifier(join_column));

  return *this;
}

query_builder &query_builder::set(std::initializer_list<key_value_pair> key_values)
{
  return set(std::vector<key_value_pair>{key_values});
}

query_builder &query_builder::set(const std::vector<key_value_pair> &key_values)
{
  transition_to(state_t::QUERY_SET);

  query_parts_.emplace_back(dialect::token_t::SET, " " + dialect_.token_at(dialect::token_t::SET) + " ");

  std::string result;
  if (key_values.size() < 2) {
    for (const auto &col: key_values) {
      result.append(dialect_.prepare_identifier(col.name()) + "=");
      auto var = col.value();
      std::visit(value_to_string_, var);
      result.append(value_to_string_.result);
    }
  } else {
    auto it = key_values.begin();
    result.append(dialect_.prepare_identifier(it->name()) + "=");
    auto var = (it++)->value();
    std::visit(value_to_string_, var);
    result.append(value_to_string_.result);
    for (; it != key_values.end(); ++it) {
      result.append(", ");
      result.append(dialect_.prepare_identifier((*it).name()) + "=");
      var = it->value();
      std::visit(value_to_string_, var);
      result.append(value_to_string_.result);
    }
  }

  query_parts_.emplace_back(dialect::token_t::UPDATE_VALUES, result);
  return *this;
}

query_builder &query_builder::where(const basic_condition &cond)
{
  transition_to(state_t::QUERY_WHERE);

  query_parts_.emplace_back(dialect::token_t::WHERE, " " + dialect_.token_at(dialect::token_t::WHERE) + " ");
  query_parts_.emplace_back(dialect::token_t::WHERE_CLAUSE, cond.evaluate(const_cast<dialect &>(dialect_), query_));

  return *this;
}

query_builder &query_builder::order_by(const std::string &column)
{
  transition_to(state_t::QUERY_ORDER_BY);

  query_parts_.emplace_back(dialect::token_t::ORDER_BY, " " + dialect_.token_at(dialect::token_t::ORDER_BY) + " " + dialect_.prepare_identifier(column));

  return *this;
}

query_builder &query_builder::group_by(const std::string &column)
{
  transition_to(state_t::QUERY_GROUP_BY);

  query_parts_.emplace_back(dialect::token_t::GROUP_BY, " " + dialect_.token_at(dialect::token_t::GROUP_BY) + " " + dialect_.prepare_identifier(column));

  return *this;
}

query_builder &query_builder::asc()
{
  transition_to(state_t::QUERY_ORDER_DIRECTION);

  query_parts_.emplace_back(dialect::token_t::ASC, " " + dialect_.token_at(dialect::token_t::ASC));

  return *this;
}

query_builder &query_builder::desc()
{
  transition_to(state_t::QUERY_ORDER_DIRECTION);

  query_parts_.emplace_back(dialect::token_t::DESC, " " + dialect_.token_at(dialect::token_t::DESC));

  return *this;
}

query_builder &query_builder::offset(size_t count)
{
  transition_to(state_t::QUERY_OFFSET);

  query_parts_.emplace_back(dialect::token_t::OFFSET, " " + dialect_.token_at(dialect::token_t::OFFSET) + " " + std::to_string(count));

  return *this;
}

query_builder &query_builder::limit(size_t count)
{
  transition_to(state_t::QUERY_LIMIT);

  query_parts_.emplace_back(dialect::token_t::LIMIT, " " + dialect_.token_at(dialect::token_t::LIMIT) + " " + std::to_string(count));

  return *this;
}

query_context query_builder::compile()
{
  for (const auto &part: query_parts_) {
    query_.sql.append(part.part);
  }
  query_.command_name = command_strings_[command_];
  return query_;
}

void query_builder::transition_to(query_builder::state_t next)
{
  if (transitions_[state_].count(next) == 0) {
    throw std::logic_error("invalid next state " + state_strings_[next]);
  }
  state_ = next;
}

void query_builder::initialize(query_builder::command_t cmd, query_builder::state_t state)
{
  command_ = cmd;
  query_ = {};
  state_ = state;
  query_parts_.clear();
}

//std::string build_create_column(const column_definition &col, const dialect &d, column_context &context)
//{
//  std::string result = d.prepare_identifier(col.name()) + " " + d.data_type_at(col.type());
//  if (col.attributes().size() > 0) {
//    result.append("(" + std::to_string(col.attributes().size()) + ")");
//  }
//  if (!col.is_nullable()) {
//    result.append(" NOT NULL");
//  }
//  if (is_constraint_set(col.attributes().options(), utils::constraints::UNIQUE)) {
//    result.append(" UNIQUE");
//  }
//  if (is_constraint_set(col.attributes().options(), utils::constraints::PRIMARY_KEY)) {
//    context.primary_keys.emplace_back(col.name());
//  }
//  if (is_constraint_set(col.attributes().options(), utils::constraints::FOREIGN_KEY)) {
//    context.foreign_contexts.push_back({col.name(), col.ref_table(), col.ref_column()});
//  }
//
//  return result;
//}

column alias(const std::string &column, const std::string &as)
{
  return {"", column, as};
}

column alias(column &&col, const std::string &as)
{
  col.as(as);
  return std::move(col);
}

column count(const std::string &column)
{
  return {sql_function_t::COUNT, column};
}

column count_all()
{
  return count("*");
}

}