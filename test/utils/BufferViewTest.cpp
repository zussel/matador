#include "BufferViewTest.hpp"

#include "matador/utils/buffer_view.hpp"

BufferViewTest::BufferViewTest()
  : matador::unit_test("buffer_view", "buffer view tests")
{
  add_test("view", [this]() { test_buffer_view(); }, "test buffer view");
}

using namespace matador;

void BufferViewTest::test_buffer_view()
{
  std::string str("hallo world");

  buffer_view bv(str);

  UNIT_ASSERT_EQUAL(11UL, bv.size());
  UNIT_ASSERT_EQUAL(11UL, bv.capacity());
  UNIT_ASSERT_EQUAL(bv.data(), str.c_str());
  UNIT_ASSERT_TRUE(bv.empty());

  bv.bump(6);

  UNIT_ASSERT_EQUAL(11UL, bv.size());
  UNIT_ASSERT_EQUAL(5UL, bv.capacity());
  UNIT_ASSERT_EQUAL(bv.data(), std::string("world"));
  UNIT_ASSERT_FALSE(bv.empty());

  bv.bump(11);

  UNIT_ASSERT_EQUAL(11UL, bv.size());
  UNIT_ASSERT_EQUAL(0UL, bv.capacity());
  UNIT_ASSERT_EQUAL(bv.data(), std::string(""));
  UNIT_ASSERT_FALSE(bv.empty());

  bv = buffer_view("foobar", 6);

  UNIT_ASSERT_EQUAL(6UL, bv.size());
  UNIT_ASSERT_EQUAL(6UL, bv.capacity());
  UNIT_ASSERT_EQUAL(bv.data(), std::string("foobar"));
  UNIT_ASSERT_TRUE(bv.empty());
}

void BufferViewTest::test_array()
{
  std::array<char, 256> data { 'H', 'a', 'l', 'l', 'o'};

  buffer_view bv(data);


}
