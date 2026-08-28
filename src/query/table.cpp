#include "matador/query/table.hpp"

#include <utility>

namespace matador::query {
table::table(const char* name)
: table(std::string(name))
{}

table::table(const std::string& name)
: table(name, name, {}) {}

table::table(const std::string& name, const std::vector<column> &columns)
: table(name, name, columns) {
}

table::table(std::string name, std::string alias, const std::vector<column> &columns)
: name_(std::move(name))
, alias_(std::move(alias))
, columns_(columns) {
  for (int i = 0; i < columns.size(); ++i) {
    columns_[i].table(this);
    // if (columns_[i].is_primary_key()) {
      // pk_column_index_ = i;
    // }
  }
}

table::table(const table &other)
: name_(other.name_)
, alias_(other.alias_)
, schema_name_(other.schema_name_)
, columns_(other.columns_)
, pk_column_index_(other.pk_column_index_) {
  for (auto &col : columns_) {
    col.table(this);
  }
}

table & table::operator=(const table &other) {
  if (this == &other) {
    return *this;
  }
  return *this = table(other);
}

table::table(table &&other) noexcept
: name_(std::move(other.name_))
, alias_(std::move(other.alias_))
, schema_name_(std::move(other.schema_name_))
, columns_(std::move(other.columns_))
, pk_column_index_(other.pk_column_index_) {
  for (auto &col : columns_) {
    col.table(this);
  }
}

table & table::operator=(table &&other) noexcept {
  name_ = std::move(other.name_);
  alias_ = std::move(other.alias_);
  schema_name_ = std::move(other.schema_name_);
  columns_ = std::move(other.columns_);
  pk_column_index_ = other.pk_column_index_;
  for (auto &col : columns_) {
    col.table(this);
  }
  return *this;
}

bool table::operator==(const table& x) const {
  return name_ == x.name_ && alias_ == x.alias_;
}

table table::as(const std::string &alias) const {
  return { name_, alias, columns_};
}

const std::string & table::table_name() const {
  return name_;
}

const std::string& table::name() const {
  return alias_;
}

const std::vector<column>& table::columns() const {
  return columns_;
}

bool table::has_alias() const {
  return name_ != alias_;
}

table::operator const std::vector<query::column>&() const {
  return columns_;
}

const column* table::operator[](const std::string &column_name) const {
  for (const auto &col : columns_) {
    if (col.column_name() == column_name) {
      return &col;
    }
  }
  return nullptr;
}

const column * table::column_by_name(const table &tab, const std::string &column_name) {
  for (const auto &col : tab.columns_) {
    if (col.column_name() == column_name) {
      return &col;
    }
  }
  return nullptr;
}

const column &table::column_ref_by_name(const table &tab, const std::string &column_name) {
  const auto *column = column_by_name(tab, column_name);
  if (column == nullptr) {
    throw std::invalid_argument("Unknown column: " + column_name);
  }
  return *column;
}

std::string table::schema_name() const {
  return schema_name_;
}

bool table::has_primary_key() const {
  return pk_column_index_ > -1;
}

const column* table::primary_key_column() const {
  return pk_column_index_ > -1 ? &columns_.at(pk_column_index_) : nullptr;
}
}
