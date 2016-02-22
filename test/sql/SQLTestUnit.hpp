//
// Created by sascha on 9/7/15.
//

#ifndef OOS_SQLTEST_HPP
#define OOS_SQLTEST_HPP


#include "unit/unit_test.hpp"

#include "sql/connection.hpp"

namespace oos {
class connection;
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
  void test_query();
  void test_query_select();

protected:
  oos::connection* create_connection();

  std::string db() const;

private:
  std::string db_;
  std::unique_ptr<oos::connection> connection_;
};


#endif //OOS_SQLTEST_HPP
