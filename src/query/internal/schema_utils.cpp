#include "matador/query/internal/schema_utils.hpp"

#include "matador/query/criteria.hpp"
#include "matador/query/generators.hpp"
#include "matador/query/internal/query_contexts.hpp"
#include "matador/query/query.hpp"

using namespace matador::utils;

namespace matador::query {
query_contexts to_query_contexts(const schema_node& node, const dialect &d) {
  query_contexts queries;

  const auto& tab = *node.info().table();
  // SELECT all
  queries.select_all = select(tab)
    .from(tab)
    .compile(d);
  if (tab.has_primary_key()) {
    // SELECT one
    queries.select_one = select(tab)
      .from(tab)
      .where(*tab.primary_key_column() == _)
      .compile(d);
    // UPDATE one
    auto update_set = query::update(tab);
    for (const auto &col: tab.columns()) {
      update_set.set(col, _);
    }
    queries.update_one = update_set.where(*tab.primary_key_column() == _)
      .compile(d);
    // DELETE one
    queries.delete_one = query::remove()
      .from(tab)
      .where(*tab.primary_key_column() == _)
      .compile(d);
  } else {
    queries.delete_one = query::remove()
      .from(tab)
      .where(*tab.join_column() == _ && *tab.inverse_join_column() == _)
      .compile(d);
  }
  // INSERT one
  std::vector<column> columns;
  for (const auto &col: tab.columns()) {
    if (col.is_primary_key() && col.options().constraints().has(column_constraint::Identity)) {
      continue;
    }
    columns.push_back(col);
  }
  if (tab.has_primary_key() && node.info().pk_generator().type() == generator_type::Identity) {
    queries.insert = query::insert()
      .into(tab, columns)
      .values(placeholders(columns.size()))
      .returning(tab.primary_key_column()->as(tab.primary_key_column()->column_name()))
      .compile(d);
  } else {
    queries.insert = query::insert()
      .into(tab, columns)
      .values(placeholders(columns.size()))
      .compile(d);
  }

  return queries;
}
}
