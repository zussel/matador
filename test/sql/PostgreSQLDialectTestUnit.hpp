//
// Created by sascha on 04.06.19.
//

#ifndef MATADOR_POSTGRESQLDIALECTTESTUNIT_HPP
#define MATADOR_POSTGRESQLDIALECTTESTUNIT_HPP


#include "matador/unit/unit_test.hpp"

class PostgreSQLDialectTestUnit : public matador::unit_test
{
public:
  PostgreSQLDialectTestUnit();

  void test_placeholder();
};


#endif //MATADOR_POSTGRESQLDIALECTTESTUNIT_HPP
