#include "matador/query/table_generator.hpp"

#include "matador/query/access.hpp"
#include "matador/query/schema.hpp"

#include <algorithm>

namespace matador::query {
table_generator::table_generator(basic_schema &repo, const std::shared_ptr<table> &t)
: repo_(repo)
, table_(t) {
}

std::shared_ptr<table> table_generator::acquire_object(basic_schema &repo, const std::type_index &ti, const std::string &name) {
  if (repo.has_table_for_type(ti)) {
    auto obj = repo.table_for_type(ti);
    repo.remove_table_for_type(ti);
    // obj->update_name(name);
    return obj;
  }

  return repo.provide_table_in_advance(ti, std::make_shared<table>(name));
}

void table_generator::on_revision(const char *id, uint64_t &rev) {
  field::attribute(*this, id, rev);
}

void table_generator::create_pk_constraint(const std::string &name) const {
  const auto pk_attr = find_column_by_name(name);
  if (pk_attr == std::end(table_->columns_)) {
    return;
  }
  constraint pk_constraint(*table_, pk_attr->index(), column_constraint::PrimaryKey);
  table_->constraints_.emplace_back(pk_constraint);
}

void table_generator::create_unique_constraint(const std::string &name) const {
  const auto pk_attr = find_column_by_name(name);
  if (pk_attr == std::end(table_->columns_)) {
    return;
  }
  constraint pk_constraint(*table_, pk_attr->index(), column_constraint::Unique);
  table_->constraints_.emplace_back(pk_constraint);
}

std::vector<column>::iterator table_generator::find_column_by_name(const std::string &name) const {
  return std::find_if(std::begin(table_->columns_), std::end(table_->columns_), [&name](const column &elem) {
    return elem.name() == name;
  });
}

void table_generator::prepare_primary_key(const column &ref, identifier &&pk) const {
  table_->pk_column_index_ = static_cast<int>(ref.index());
  // table_->pk_identifier_ = std::move(pk);
}

void table_generator::prepare_relation_table(const std::string &join_column, const std::string &inverse_join_column) const {
  auto it = find_column_by_name(join_column);
  if (it == std::end(table_->columns_)) {
    return;
  }
  table_->join_column_index_ = it->index();

  it = find_column_by_name(inverse_join_column);
  if (it == std::end(table_->columns_)) {
    return;
  }
  table_->inverse_join_column_index_ = it->index();
}
}
