//
// Created by sascha on 9/7/15.
//

#ifndef OOS_SQLTEST_HPP
#define OOS_SQLTEST_HPP


#include "object/object_store.hpp"
#include "unit/unit_test.hpp"

namespace oos {
class session;
}

class SQLTestUnit : public oos::unit_test
{
public:
  SQLTestUnit(const std::string &name, const std::string &msg, const std::string &db = "memory");
  virtual ~SQLTestUnit();

  virtual void initialize();
  virtual void finalize();

  void test_create();
  void test_statement();
  void test_foreign_query();

protected:
  oos::session* create_session();

  oos::object_store& ostore();
  const oos::object_store& ostore() const;

  std::string db() const;

private:
  oos::object_store ostore_;
  std::string db_;
  std::unique_ptr<oos::session> session_;
};


#endif //OOS_SQLTEST_HPP
