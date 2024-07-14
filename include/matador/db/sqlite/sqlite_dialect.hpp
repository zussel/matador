#ifndef OOS_SQLITE_DIALECT_HPP
#define OOS_SQLITE_DIALECT_HPP

#include "matador/sql/basic_dialect.hpp"

namespace matador::sqlite {

class sqlite_dialect : public basic_dialect
{
public:
  sqlite_dialect();

  [[nodiscard]] const char* to_database_type_string(matador::data_type type) const override;
  data_type string_type(const char *type) const;

  [[nodiscard]] dialect_traits::identifier identifier_escape_type() const override;
};

}


#endif //OOS_SQLITE_DIALECT_HPP
