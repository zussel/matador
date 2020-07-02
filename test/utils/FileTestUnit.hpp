#ifndef MATADOR_FILETESTUNIT_HPP
#define MATADOR_FILETESTUNIT_HPP

#include "matador/unit/unit_test.hpp"

class FileTestUnit : public matador::unit_test
{
public:
  FileTestUnit();

  void test_file();
};


#endif //MATADOR_FILETESTUNIT_HPP
