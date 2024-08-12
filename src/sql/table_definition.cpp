#include "matador/sql/table_definition.hpp"

namespace matador::sql {
table_definition::table_definition(std::initializer_list<column_definition> columns)
: columns_(columns)
{
  init();
}

table_definition::table_definition(const std::vector<column_definition> &columns)
: columns_(columns)
{
  init();
}

table_definition::table_definition(const table_definition &x)
: columns_(x.columns_)
, pk_index_(x.pk_index_)
{
  for (auto& col : columns_) {
    add_to_map(col, col.index());
  }
}

table_definition &table_definition::operator=(const table_definition &x)
{
  if (&x == this) {
    return *this;
  }

  columns_ = x.columns_;
  columns_by_name_.clear();
  pk_index_ = x.pk_index_;
  for (auto& col : columns_) {
    add_to_map(col, col.index());
  }
  return *this;
}

bool table_definition::has_primary_key() const
{
    return pk_index_ > -1;
}

std::optional<column_definition> table_definition::primary_key() const
{
  if (!has_primary_key()) {
    return std::nullopt;
  }

  return columns_[pk_index_];
}

void table_definition::append(column_definition col)
{
  auto &ref = columns_.emplace_back(std::move(col));
  add_to_map(ref, columns_.size()-1);
}

const std::vector<column_definition> &table_definition::columns() const
{
  return columns_;
}

const column_definition &table_definition::at(const column &col) const
{
  return columns_by_name_.at(col.name).first;
}

const column_definition &table_definition::at(size_t index) const
{
  return columns_.at(index);
}

table_definition::iterator table_definition::find(const std::string &column_name)
{
  auto it = columns_by_name_.find(column_name);
  return it != columns_by_name_.end() ? columns_.begin() + it->second.second : columns_.end();
}

table_definition::const_iterator table_definition::find(const std::string &column_name) const {
  auto it = columns_by_name_.find(column_name);
  return it != columns_by_name_.end() ? columns_.begin() + it->second.second : columns_.end();
}

table_definition::iterator table_definition::begin()
{
  return columns_.begin();
}

table_definition::const_iterator table_definition::begin() const
{
  return columns_.begin();
}

table_definition::const_iterator table_definition::cbegin() const
{
  return columns_.cbegin();
}

table_definition::iterator table_definition::end()
{
  return columns_.end();
}

table_definition::const_iterator table_definition::end() const
{
  return columns_.end();
}

table_definition::const_iterator table_definition::cend() const
{
  return columns_.cend();
}

size_t table_definition::size() const
{
  return columns_.size();
}

bool table_definition::empty() const
{
  return columns_.empty();
}

void table_definition::clear()
{
  columns_.clear();
  columns_by_name_.clear();
}

void table_definition::init()
{
  size_t index{0};
  for(auto &col : columns_) {
    add_to_map(col, index++);
  }
}

void table_definition::add_to_map(column_definition &col, size_t index)
{
  columns_by_name_.emplace(col.name(), column_index_pair {std::ref(col), index});
  if (utils::is_constraint_set(col.attributes().options(), utils::constraints::PRIMARY_KEY)) {
    pk_index_ = static_cast<int>(index);
  }
}

}