#include "matador/query/dialect.hpp"

#include "matador/utils/string.hpp"

namespace matador::query {
const std::string &dialect::token_at(const dialect_token token) const {
  return tokens_.at(token);
}

const std::string &dialect::data_type_at(const utils::basic_type type) const {
  return data_types_.at(type);
}

const std::string &dialect::sql_function_at(const query_functions func) const {
  return sql_func_map_.at(func);
}

const std::string &dialect::to_string(const bool val) const {
  return bool_strings_[static_cast<size_t>(val)];
}

void dialect::bool_strings(const std::string &true_string, const std::string &false_string) {
  bool_strings_[0] = false_string;
  bool_strings_[1] = true_string;
}

dialect::escape_identifier_t dialect::identifier_escape_type() const {
  return identifier_escape_type_;
}

void dialect::identifier_escape_type(const escape_identifier_t escape_identifier) {
  identifier_escape_type_ = escape_identifier;
}

std::string dialect::next_placeholder(const std::vector<std::string> &bind_vars) const {
  return placeholder_func_(bind_vars.size());
}

std::string dialect::to_escaped_string(const utils::blob_type_t &value) const {
  return to_escaped_string_func_(value);
}

std::string dialect::default_schema_name() const {
  return default_schema_name_;
}

const std::string &dialect::add_constraint() const {
  return token_at(dialect_token::AddConstraint);
}

const std::string &dialect::alter() const {
  return token_at(dialect_token::Alter);
}

const std::string &dialect::and_() const {
  return token_at(dialect_token::And);
}

const std::string &dialect::as() const {
  return token_at(dialect_token::As);
}

const std::string &dialect::asc() const {
  return token_at(dialect_token::Asc);
}

const std::string &dialect::begin() const {
  return token_at(dialect_token::Begin);
}

const std::string &dialect::asterisk() const {
  return token_at(dialect_token::Asterisk);
}

const std::string &dialect::begin_binary_data() const {
  return token_at(dialect_token::BeginBinaryData);
}

const std::string &dialect::begin_string_data() const {
  return token_at(dialect_token::BeginStringData);
}

const std::string &dialect::between() const {
  return token_at(dialect_token::Between);
}

const std::string &dialect::column() const {
  return token_at(dialect_token::Column);
}

const std::string &dialect::columns() const {
  return token_at(dialect_token::Columns);
}

const std::string &dialect::commit() const {
  return token_at(dialect_token::Commit);
}

const std::string &dialect::constraint() const {
  return token_at(dialect_token::Constraint);
}

const std::string &dialect::create() const {
  return token_at(dialect_token::Create);
}

const std::string &dialect::drop() const {
  return token_at(dialect_token::Drop);
}

const std::string &dialect::end_binary_data() const {
  return token_at(dialect_token::EndBinaryData);
}

const std::string &dialect::end_string_data() const {
  return token_at(dialect_token::EndStringData);
}

const std::string& dialect::exists() const {
  return token_at(dialect_token::Exists);
}

const std::string &dialect::from() const {
  return token_at(dialect_token::From);
}

const std::string &dialect::identity() const {
  return token_at(dialect_token::Identity);
}

const std::string &dialect::in() const {
  return token_at(dialect_token::In);
}

const std::string &dialect::desc() const {
  return token_at(dialect_token::Desc);
}

const std::string &dialect::distinct() const {
  return token_at(dialect_token::Distinct);
}

const std::string &dialect::drop_constraint() const {
  return token_at(dialect_token::DropConstraint);
}

const std::string &dialect::end_quote() const {
  return token_at(dialect_token::EndQuote);
}

const std::string &dialect::foreign_key() const {
  return token_at(dialect_token::ForeignKey);
}

const std::string &dialect::group_by() const {
  return token_at(dialect_token::GroupBy);
}

const std::string &dialect::insert() const {
  return token_at(dialect_token::Insert);
}

const std::string &dialect::into() const {
  return token_at(dialect_token::Into);
}

const std::string &dialect::is_null() const {
  return token_at(dialect_token::IsNull);
}

const std::string &dialect::is_not_null() const {
  return token_at(dialect_token::IsNotNull);
}

const std::string &dialect::join() const {
  return token_at(dialect_token::Join);
}

const std::string &dialect::like() const {
  return token_at(dialect_token::Like);
}

const std::string &dialect::limit() const {
  return token_at(dialect_token::Limit);
}

const std::string& dialect::nextval() const {
  return token_at(dialect_token::NextVal);
}

const std::string& dialect::currval() const {
  return token_at(dialect_token::CurrVal);
}

const std::string &dialect::not_() const {
  return token_at(dialect_token::Not);
}

const std::string &dialect::not_null() const {
  return token_at(dialect_token::NotNull);
}

const std::string &dialect::offset() const {
  return token_at(dialect_token::Offset);
}

const std::string &dialect::on() const {
  return token_at(dialect_token::On);
}

const std::string &dialect::or_() const {
  return token_at(dialect_token::Or);
}

const std::string &dialect::order_by() const {
  return token_at(dialect_token::OrderBy);
}

const std::string &dialect::primary_key() const {
  return token_at(dialect_token::PrimaryKey);
}

const std::string &dialect::references() const {
  return token_at(dialect_token::References);
}

const std::string &dialect::remove() const {
  return token_at(dialect_token::Remove);
}

const std::string & dialect::returning() const {
  return token_at(dialect_token::Returning);
}

const std::string &dialect::rollback() const {
  return token_at(dialect_token::Rollback);
}

const std::string &dialect::schema() const {
  return token_at(dialect_token::Schema);
}

const std::string &dialect::select() const {
  return token_at(dialect_token::Select);
}

const std::string &dialect::set() const {
  return token_at(dialect_token::Set);
}

const std::string& dialect::sequence() const {
  return token_at(dialect_token::Sequence);
}

const std::string &dialect::start_quote() const {
  return token_at(dialect_token::StartQuote);
}

const std::string &dialect::string_quote() const {
  return token_at(dialect_token::StringQuote);
}

const std::string &dialect::table() const {
  return token_at(dialect_token::Table);
}

const std::string &dialect::unique() const {
  return token_at(dialect_token::Unique);
}

const std::string &dialect::update() const {
  return token_at(dialect_token::Update);
}

const std::string &dialect::values() const {
  return token_at(dialect_token::Values);
}

const std::string &dialect::where() const {
  return token_at(dialect_token::Where);
}
}
