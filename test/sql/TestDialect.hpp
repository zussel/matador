#ifndef OOS_TESTDIALECT_HPP
#define OOS_TESTDIALECT_HPP

#include "matador/sql/basic_dialect.hpp"

#include "matador/utils/data_types.hpp"

class TestDialect : public matador::basic_dialect
{
public:

  TestDialect();

  const char* to_database_type_string(matador::data_type type) const override;

  matador::dialect_traits::identifier identifier_escape_type() const override;
};

#endif //OOS_TESTDIALECT_HPP
