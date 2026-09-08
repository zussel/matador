#include "matador/query/record.hpp"

#include <algorithm>
#include <stdexcept>

namespace matador::query {
record::record(const std::initializer_list<field> columns) {
  for (auto &&col :columns) {
    index_by_name_.emplace(col.name(), fields_.size());
    fields_.push_back(col);
  }
}

record::record(const std::vector<field> &columns)
: fields_(columns) {
  for (auto i = 0u; i < columns.size(); ++i) {
    index_by_name_.emplace(columns[i].name(), i);
  }
}

record &record::operator=(const record &x) {
  if (&x == this) {
    return *this;
  }

  fields_ = x.fields_;
  index_by_name_ = x.index_by_name_;
  return *this;
}

const std::vector<field> &record::columns() const {
  return fields_;
}

const field &record::at(const std::string &name) const {
  const auto it = index_by_name_.find(name);
  if (it == index_by_name_.end()) {
    throw std::out_of_range("Column not found");
  }

  return fields_[it->second];
}

const field &record::at(const size_t index) const {
  return fields_.at(index);
}

field &record::at(const std::string &name) {
  const auto it = index_by_name_.find(name);
  if (it == index_by_name_.end()) {
    throw std::out_of_range("Column not found");
  }

  return fields_[it->second];
}

field &record::at(const size_t index) {
  return fields_.at(index);
}

record::iterator record::find(const std::string &column_name) {
  const auto it = index_by_name_.find(column_name);
  return it != index_by_name_.end() ? fields_.begin() + static_cast<long>(it->second) : fields_.end();
}

record::const_iterator record::find(const std::string &column_name) const {
  const auto it = index_by_name_.find(column_name);
  return it != index_by_name_.end() ? fields_.begin() + static_cast<long>(it->second) : fields_.end();
}

bool record::operator==(const record &rhs) const {
  if (size() != rhs.size()) {
    return false;
  }
  for(size_t i = 0; i < size(); ++i) {
    if (at(i) != rhs.at(i)) {
      return false;
    }
  }
  return true;
}

bool record::operator!=(const record &rhs) const {
  return !operator==(rhs);
}

void record::append(field col) {
  index_by_name_.emplace(col.name(), fields_.size());
  fields_.push_back(std::move(col));
}

record::iterator record::begin() {
  return fields_.begin();
}

record::const_iterator record::begin() const {
  return fields_.begin();
}

record::const_iterator record::cbegin() const {
  return fields_.cbegin();
}

record::iterator record::end() {
  return fields_.end();
}

record::const_iterator record::end() const {
  return fields_.end();
}

record::const_iterator record::cend() const {
  return fields_.cend();
}

size_t record::size() const {
  return fields_.size();
}

bool record::empty() const {
  return fields_.empty();
}

void record::clear() {
  fields_.clear();
  index_by_name_.clear();
}
}
