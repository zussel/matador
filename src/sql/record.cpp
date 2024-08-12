#include "matador/sql/record.hpp"
#include "matador/sql/column.hpp"

#include <algorithm>
#include <stdexcept>

namespace matador::sql {

record::record(std::initializer_list<field> columns)
{
  for (auto &&col :columns) {
    const auto it = fields_by_name_.emplace(col.name(), field_index_pair {col, fields_.size()});
    fields_.push_back(std::ref(it.first->second.first));
  }
}

record::record(const std::vector<field> &columns)
{
  for (auto &&col :columns) {
    const auto it = fields_by_name_.emplace(col.name(), field_index_pair {col, fields_.size()});
    fields_.push_back(std::ref(it.first->second.first));
  }
}

record::record(const record &x)
: fields_by_name_(x.fields_by_name_)
//, pk_index_(x.pk_index_)
{
  for (auto& col : x.fields_) {
    auto &it = fields_by_name_.at(col.get().name());
    fields_.push_back(std::ref(it.first));
  }
}

record &record::operator=(const record &x)
{
  if (&x == this) {
    return *this;
  }

  fields_by_name_ = x.fields_by_name_;
  fields_.clear();
//  pk_index_ = x.pk_index_;
  for (auto& col : x.fields_) {
    auto &it = fields_by_name_.at(col.get().name());
    fields_.push_back(std::ref(it.first));
  }
  return *this;
}

const std::vector<record::field_ref> &record::columns() const
{
  return fields_;
}

//bool record::has_primary_key() const
//{
//  return pk_index_ > -1;
//}
//
//std::optional<field> record::primary_key() const
//{
//  if (!has_primary_key()) {
//    return std::nullopt;
//  }
//
//  return columns_[pk_index_];
//}

const field &record::at(const column &col) const
{
  const auto &res = fields_by_name_.at(col.name);
  const auto &f = res.first;
  return f;
}

const field &record::at(size_t index) const
{
  return fields_.at(index);
}

record::iterator record::find(const std::string &column_name)
{
  auto it = fields_by_name_.find(column_name);
  return it != fields_by_name_.end() ? fields_.begin() + it->second.second : fields_.end();
}

record::const_iterator record::find(const std::string &column_name) const {
  auto it = fields_by_name_.find(column_name);
  return it != fields_by_name_.end() ? fields_.begin() + it->second.second : fields_.end();
}

void record::append(const field &col)
{
  const auto it = fields_by_name_.emplace(col.name(), field_index_pair {col, fields_.size()});
  fields_.push_back(std::ref(it.first->second.first));
}

record::iterator record::begin()
{
  return fields_.begin();
}

record::const_iterator record::begin() const
{
  return fields_.begin();
}

record::const_iterator record::cbegin() const
{
  return fields_.cbegin();
}

record::iterator record::end()
{
  return fields_.end();
}

record::const_iterator record::end() const
{
  return fields_.end();
}

record::const_iterator record::cend() const
{
  return fields_.cend();
}

size_t record::size() const
{
  return fields_.size();
}

bool record::empty() const
{
  return fields_.empty();
}

void record::clear()
{
  fields_.clear();
  fields_by_name_.clear();
}

//bool record::unknown() const
//{
//  return std::all_of(std::begin(columns_), std::end(columns_), [](const auto &col) {
//    return col.type() == data_type_t::type_unknown;
//  });
//}

void record::init()
{
  size_t index{0};
  for(auto &col : fields_) {
    add_to_map(col, index++);
  }
}

void record::add_to_map(field &col, size_t index)
{
  fields_by_name_.emplace(col.name(), field_index_pair {std::ref(col), index});
}

}
