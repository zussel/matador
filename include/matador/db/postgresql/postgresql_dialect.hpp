//
// Created by sascha on 24.05.19.
//

#ifndef MATADOR_POSTGRESQL_DIALECT_HPP
#define MATADOR_POSTGRESQL_DIALECT_HPP

#include <matador/sql/basic_dialect.hpp>

namespace matador {
namespace postgresql {

class postgresql_dialect : public basic_dialect
{
public:
  postgresql_dialect();

  const char *to_database_type_string(matador::data_type type) const override;

  database_type string_type(const char *type) const;

  dialect_traits::identifier identifier_escape_type() const override;

  std::string next_placeholder() const override;
};

}
}

#endif //MATADOR_POSTGRESQL_DIALECT_HPP
