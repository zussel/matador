#include "StreamsTest.hpp"

#include "matador/utils/stream.hpp"

#include <list>

using namespace matador;

StreamsTest::StreamsTest()
  : unit_test("streams", "streams test")
{
  add_test("iterate", std::bind(&StreamsTest::test_iterate, this), "streams iterate test");
}

void StreamsTest::test_iterate()
{
  std::list<int> ints = { 1, 2, 3, 4 };
  std::list<int> result;

//  auto s = make_stream(std::begin(ints), std::end(ints));
  auto s = make_stream(1, 4);

  for(const auto &i : s) {
    result.push_back(i);
  }

  UNIT_ASSERT_EQUAL(4UL, result.size());

  s = make_stream(1, 4);

  auto first = s.begin();

  UNIT_ASSERT_EQUAL(1, *first);

  ++first;

  UNIT_ASSERT_EQUAL(2, *first);

  auto next = first++;

  UNIT_ASSERT_EQUAL(2, *next);
}
