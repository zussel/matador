#ifndef TRANSACTION_TEST_UNIT_HPP
#define TRANSACTION_TEST_UNIT_HPP

#include "matador/object/object_store.hpp"

#include "matador/unit/unit_test.hpp"

namespace matador {
class session;
}

class TransactionTestUnit : public matador::unit_test
{
public:
  explicit TransactionTestUnit(const std::string &prefix, std::string dns = "memory");
  ~TransactionTestUnit() override = default;

  void test_simple();
  void test_nested();
  void test_foreign();
  void test_has_many_list_commit();
  void test_has_many_list_rollback();
  void test_has_many_list();
  void test_has_many_vector();

private:
  std::string dns_;

};

#endif /* TRANSACTION_TEST_UNIT_HPP */
