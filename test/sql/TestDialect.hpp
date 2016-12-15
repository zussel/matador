//
// Created by sascha on 2/26/16.
//

#ifndef OOS_TESTDIALECT_HPP
#define OOS_TESTDIALECT_HPP


#include "oos/sql/types.hpp"
#include "oos/sql/basic_dialect.hpp"

class TestDialect : public oos::basic_dialect
{
public:

  TestDialect();

  const char* type_string(oos::data_type type) const override;

  oos::dialect_traits::identifier identifier_escape_type() const override;
};

#endif //OOS_TESTDIALECT_HPP
