#ifndef OOS_ORMRELATIONTESTUNIT_HPP
#define OOS_ORMRELATIONTESTUNIT_HPP

#include "matador/unit/unit_test.hpp"

class OrmRelationTestUnit : public matador::unit_test
{
public:
  OrmRelationTestUnit(const std::string &prefix, std::string dns);

  void test_has_many_varchars();
  void test_has_many_ints();
  void test_has_many_delete();
  void test_belongs_to();
  void test_many_to_many();
  void test_save();
  void test_save_object();

private:
  std::string dns_;
};

#endif //OOS_ORMRELATIONTESTUNIT_HPP
