//
// Created by sascha on 2/26/16.
//

#ifndef OOS_TESTDIALECT_HPP
#define OOS_TESTDIALECT_HPP


#include "sql/types.hpp"
#include "sql/basic_dialect.hpp"

class TestDialect : public oos::basic_dialect
{
public:

  TestDialect() { }

  const char* type_string(oos::data_type_t type) const;
};

#endif //OOS_TESTDIALECT_HPP
