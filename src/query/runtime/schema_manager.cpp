#include "matador/query/runtime/schema_manager.hpp"

#include "matador/query/builder/builder.hpp"
#include "matador/query/builder/query.hpp"

#include "matador/query/schema/basic_schema.hpp"

#include "matador/query/model/constraint.hpp"

#include "matador/query/runtime/connection.hpp"

#include <iostream>

namespace matador::query {
namespace {
result<void, error> create_tables(const basic_schema &repo, const connection &conn);
result<void, error> create_primary_key_constraints(const basic_schema &repo, const connection &conn);
result<void, error> create_foreign_key_constraints(const basic_schema &repo, const connection &conn);
result<void, error> setup_primary_key_generator(const basic_schema &scm, const connection &conn);

result<void, error> drop_tables(const basic_schema &repo, const connection &conn);
result<void, error> drop_primary_key_constraints(const basic_schema &repo, const connection &conn);
result<void, error> drop_foreign_key_constraints(const basic_schema &repo, const connection &conn);
result<void, error> cleanup_primary_key_generator(const basic_schema &scm, const connection &conn);
}
schema_manager::schema_manager(basic_schema &scm)
: schema_(scm) {}

result<void, error> schema_manager::create(const connection &conn) const {
  std::vector<std::string> fk_sql_commands;

  const auto q = query::create().schema(schema_.name()).compile(conn.dialect());
  std::cout << q.sql << std::endl;

  return create_tables(schema_, conn)
    .and_then([this, &conn]{ return create_primary_key_constraints(schema_, conn);})
    .and_then([this, &conn]{ return create_foreign_key_constraints(schema_, conn);})
    .and_then([this, &conn]{ return setup_primary_key_generator(schema_, conn);});
}

result<void, error> schema_manager::drop(const connection &conn) const {
  return drop_primary_key_constraints(schema_, conn)
    .and_then([this, &conn]{ return drop_foreign_key_constraints(schema_, conn);})
    .and_then([this, &conn]{ return drop_tables(schema_, conn);})
    .and_then([this, &conn]{ return cleanup_primary_key_generator(schema_, conn);});
}
result<bool, error> schema_manager::table_exists(const std::string &table_name, const connection &conn) const {
  return conn.exists(schema_.name(), table_name);
}

namespace {
query_context build_add_constraint_context(const schema_node &node,
                                          const class constraint &cons,
                                          const dialect &d) {
  if (cons.is_foreign_key_constraint()) {
    return alter()
        .table(node.name())
        .add_constraint(cons)
        .compile(d);
  }
  if (cons.is_primary_key_constraint()) {
    return alter()
        .table(node.name())
        .add_constraint(cons)
        .compile(d);
  }
  return {};
}

query_context build_drop_constraint_context(const schema_node &node,
                                            const class constraint &cons,
                                            const dialect &d) {
  return alter()
      .table(node.name())
      .drop_constraint(cons)
      .compile(d);
}

result<void, error> create_tables(const basic_schema &repo, const connection &conn) {
  for (const auto &node: repo) {
    auto ctx = query::create()
        .table(node.name())
        .columns(node.info().columns())
        .compile(conn.dialect());

    if (auto result = conn.execute(ctx); !result) {
      return failure(result.err());
    }
  }

  return ok<void>();
}

result<void, error> create_primary_key_constraints(const basic_schema &repo, const connection &conn) {
  for (const auto &node: repo) {
    for (const auto &cons: node.info().constraints()) {
      if (!cons.is_primary_key_constraint()) {
        continue;
      }
      auto ctx = build_add_constraint_context(node, cons, conn.dialect());

      if (auto result = conn.execute(ctx); !result) {
        return failure(result.err());
      }
    }
  }

  return ok<void>();
}

result<void, error> create_foreign_key_constraints(const basic_schema &repo, const connection &conn) {
  for (const auto &node: repo) {
    for (const auto &cons: node.info().constraints()) {
      if (cons.is_primary_key_constraint()) {
        continue;
      }
      auto ctx = build_add_constraint_context(node, cons, conn.dialect());

      if (const auto result = conn.execute(ctx); !result) {
        return failure(result.err());
      }
    }
  }

  return ok<void>();
}

result<void, error> setup_primary_key_generator(const basic_schema &scm, const connection& conn) {
  for (const auto &node : scm) {
    if (node.info().pk_generator().type() == generator_type::Sequence) {
      const auto result = create().sequence(node.name() + "_pk_seq").execute(conn);
      if (!result) {
        return failure(result.err());
      }
    } else if (node.info().pk_generator().type() == generator_type::Table) {
      if (const auto result = conn.exists(scm.name(), "sequence_table"); result) {
        if (!*result) {
          const auto exec_result = create()
            .table("sequence_table")
            .columns({
              col("name", utils::basic_type::Varchar, 255).not_null().primary_key(),
              col("next_id", utils::basic_type::Int64)
            })
            .execute(conn);
          if (!exec_result) {
            return failure(exec_result.err());
          }
        }
      } else {
        return failure(result.err());
      }

      const auto insert_result = insert().into("sequence_table", {"name", "next_id"}).values({node.name(), 1}).execute(conn);
      if (!insert_result) {
        return failure(insert_result.err());
      }
    }
  }

  return ok<void>();
}

result<void, error> drop_tables(const basic_schema& repo, const connection& conn) {
  for (const auto &node: repo) {
    auto ctx = query::drop()
        .table(node.name())
        .compile(conn.dialect());

    if (auto result = conn.execute(ctx); !result) {
      return failure(result.err());
    }
  }

  return ok<void>();
}

result<void, error> drop_primary_key_constraints(const basic_schema& repo, const connection& conn) {
  for (const auto &node: repo) {
    for (const auto &cons: node.info().constraints()) {
      if (cons.is_primary_key_constraint()) {
        continue;
      }
      auto ctx = alter()
          .table(node.name())
          .drop_constraint(cons)
          .compile(conn.dialect());

      if (auto result = conn.execute(ctx); !result) {
        return failure(result.err());
      }
    }
  }

  return ok<void>();
}

result<void, error> drop_foreign_key_constraints(const basic_schema& repo, const connection& conn) {
  for (const auto &node: repo) {
    for (const auto &cons: node.info().constraints()) {
      if (!cons.is_primary_key_constraint()) {
        continue;
      }
      auto ctx = alter()
          .table(node.name())
          .drop_constraint(cons)
          .compile(conn.dialect());

      if (auto result = conn.execute(ctx); !result) {
        return failure(result.err());
      }
    }
  }

  return ok<void>();
}

result<void, error> cleanup_primary_key_generator(const basic_schema& scm, const connection& conn) {
  for (const auto &node : scm) {
    if (node.info().pk_generator().type() == generator_type::Sequence) {
      const auto result = drop().sequence(node.name() + "_pk_seq").execute(conn);
      if (!result) {
        return failure(result.err());
      }
    } else if (node.info().pk_generator().type() == generator_type::Table) {
      if (const auto result = conn.exists(scm.name(), "sequence_table"); result) {
        if (*result) {
          const auto drop_result = drop().table("sequence_table").execute(conn);
          if (!drop_result) {
            return failure(result.err());
          }
        }
      } else {
        return failure(result.err());
      }
    }
  }

  return ok<void>();
}
}
} // namespace matador::query