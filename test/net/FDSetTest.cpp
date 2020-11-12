#include "FDSetTest.hpp"

#include "matador/net/fdset.hpp"
#include "matador/net/select_fdsets.hpp"

using namespace matador;

FDSetTest::FDSetTest()
  : matador::unit_test("fdset", "fdset test unit")
{
  add_test("fdset", std::bind(&FDSetTest::test_fdset, this), "fdset test");
  add_test("select_fdset", std::bind(&FDSetTest::test_select_fdsets, this), "select fdset test");
}

struct select_fd_sets_const_tester
{
  const fdset& read_set() const { return fdsets.read_set(); }
  const fdset& write_set() const { return fdsets.write_set(); }
  const fdset& except_set() const { return fdsets.except_set(); }

  select_fdsets fdsets;
};

void FDSetTest::test_fdset()
{
  fdset fset;

  size_t empty_count(0);
  size_t filled_count(3);

  UNIT_ASSERT_EQUAL(empty_count, fset.count());
  UNIT_ASSERT_TRUE(fset.empty());

  fset.set(7);
  fset.set(3);
  fset.set(5);

  UNIT_ASSERT_EQUAL(filled_count, fset.count());
  UNIT_ASSERT_EQUAL(7, fset.maxp1());
  UNIT_ASSERT_TRUE(fset.is_set(3));
  UNIT_ASSERT_TRUE(fset.is_set(5));
  UNIT_ASSERT_TRUE(fset.is_set(7));

  fset.clear(3);

  UNIT_ASSERT_FALSE(fset.is_set(3));

  fset.reset();

  UNIT_ASSERT_FALSE(fset.is_set(5));
  UNIT_ASSERT_FALSE(fset.is_set(7));
}

void FDSetTest::test_select_fdsets()
{
  select_fdsets selectfds;

  selectfds.set(select_fdsets::read_type, 7);
  selectfds.set(select_fdsets::read_type, 3);
  selectfds.set(select_fdsets::read_type, 5);
  selectfds.set(select_fdsets::write_type, 6);
  selectfds.set(select_fdsets::write_type, 4);
  selectfds.set(select_fdsets::write_type, 2);
  selectfds.set(select_fdsets::except_type, 1);
  selectfds.set(select_fdsets::except_type, 8);

  size_t read_set_count(3);
  size_t write_set_count(3);
  size_t except_set_count(2);
  UNIT_ASSERT_EQUAL(8, selectfds.maxp1());
  UNIT_ASSERT_EQUAL(read_set_count, selectfds.read_set().count());
  UNIT_ASSERT_EQUAL(read_set_count, selectfds.fd_set(selectfds.read_type).count());
  UNIT_ASSERT_EQUAL(write_set_count, selectfds.write_set().count());
  UNIT_ASSERT_EQUAL(write_set_count, selectfds.fd_set(selectfds.write_type).count());
  UNIT_ASSERT_EQUAL(except_set_count, selectfds.except_set().count());
  UNIT_ASSERT_EQUAL(except_set_count, selectfds.fd_set(selectfds.except_type).count());

  UNIT_ASSERT_TRUE(selectfds.is_set(selectfds.read_type, 3));
  UNIT_ASSERT_FALSE(selectfds.is_set(selectfds.read_type, 1));

  UNIT_ASSERT_TRUE(selectfds.is_set(selectfds.write_type, 6));
  selectfds.clear(selectfds.write_type, 6);
  UNIT_ASSERT_FALSE(selectfds.is_set(selectfds.write_type, 6));

  selectfds.reset(selectfds.read_type);
  UNIT_ASSERT_EQUAL(0UL, selectfds.read_set().count());

  // const call tests
  select_fd_sets_const_tester const_tester;

  UNIT_ASSERT_TRUE(const_tester.read_set().empty());
  UNIT_ASSERT_TRUE(const_tester.write_set().empty());
  UNIT_ASSERT_TRUE(const_tester.except_set().empty());
}
