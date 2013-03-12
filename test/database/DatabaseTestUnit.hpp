#ifndef DATABASE_TEST_UNIT_HPP
#define DATABASE_TEST_UNIT_HPP

#include "object/object_store.hpp"

#include "unit/unit_test.hpp"

namespace oos {
class session;
}

class DatabaseTestUnit : public oos::unit_test
{
protected:
  DatabaseTestUnit(const std::string &msg, const std::string &db);

public:
  virtual ~DatabaseTestUnit();

  virtual void initialize();
  virtual void finalize();

  void test_open_close();
  void test_create_drop();
  void test_drop();
  void test_reopen();
  void test_insert();
  void test_simple();
  void test_with_sub();
  void test_with_list();
  void test_with_vector();
  void test_reload();
  void test_reload_container();

protected:
  oos::session* create_session();

  oos::object_store& ostore();
  const oos::object_store& ostore() const;
  
private:
  oos::object_store ostore_;
  std::string db_;
};

#endif /* DATABASE_TEST_UNIT_HPP */
