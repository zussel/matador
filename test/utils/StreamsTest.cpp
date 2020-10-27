#include "StreamsTest.hpp"

#include "matador/utils/stream.hpp"

#include <list>

using namespace matador;

StreamsTest::StreamsTest()
  : unit_test("streams", "streams test")
{
  add_test("iterate", std::bind(&StreamsTest::test_iterate, this), "streams iterate test");
  add_test("min", std::bind(&StreamsTest::test_min, this), "streams find minimum test");
  add_test("filter", std::bind(&StreamsTest::test_filter, this), "streams filter elements test");
  add_test("map", std::bind(&StreamsTest::test_map, this), "streams map elements test");
  add_test("take", std::bind(&StreamsTest::test_take, this), "streams take elements test");
  add_test("skip", std::bind(&StreamsTest::test_skip, this), "streams skip elements test");
  add_test("every", std::bind(&StreamsTest::test_every, this), "streams every elements test");
}

bool is_even(int val) { return val % 2 == 0; }

namespace testing {

struct person
{
  person(std::string n, int a) : name(std::move(n)), age(a)
  {}

  std::string name;
  int age = 0;
};
}

void StreamsTest::test_iterate()
{
  std::list<int> ints = { 1, 2, 3, 4 };

  auto s = make_stream(ints);

  auto result = s.collect<std::list>();

  UNIT_ASSERT_EQUAL(4UL, result.size());

  s = make_stream(1, 4);

  auto first = s.begin();

  UNIT_ASSERT_EQUAL(1, *first);

  ++first;

  UNIT_ASSERT_EQUAL(2, *first);

  auto current = first++;

  UNIT_ASSERT_EQUAL(2, *current);
}

void StreamsTest::test_min()
{
  auto minval = make_stream(1, 8).min();

  UNIT_ASSERT_TRUE(minval.has_value());
  UNIT_ASSERT_EQUAL(1, minval.value());
}

void StreamsTest::test_filter()
{
  auto result = make_stream(1, 8)
    .filter(is_even)
    .collect<std::vector>();

  UNIT_ASSERT_EQUAL(4UL, result.size());
  UNIT_ASSERT_EQUAL(2, result.at(0));
  UNIT_ASSERT_EQUAL(4, result.at(1));
  UNIT_ASSERT_EQUAL(6, result.at(2));
  UNIT_ASSERT_EQUAL(8, result.at(3));
}

void StreamsTest::test_map()
{
  auto result = make_stream<::testing::person>({
       {"otto",   34},
       {"jane",   27},
       {"george", 56},
       {"bobby",  15}
     }).map([](const ::testing::person &p) {
       return p.name;
     }).collect<std::vector>();

  UNIT_ASSERT_EQUAL(4UL, result.size());
  UNIT_ASSERT_EQUAL("otto", result.at(0));
  UNIT_ASSERT_EQUAL("jane", result.at(1));
  UNIT_ASSERT_EQUAL("george", result.at(2));
  UNIT_ASSERT_EQUAL("bobby", result.at(3));
}

void StreamsTest::test_take()
{
  auto result = make_stream_counter(6)
    .filter(is_even)
    .take(10)
    .collect<std::vector>();

  auto expected_result = std::vector<int>({6, 8, 10, 12, 14, 16, 18, 20, 22, 24});

  UNIT_ASSERT_EQUAL(10UL, result.size());
  UNIT_ASSERT_TRUE(expected_result == result);
}

void StreamsTest::test_skip()
{
  auto result = make_stream(1, 8)
    .filter(is_even)
    .skip(2)
    .collect<std::vector>();

  auto expected_result = std::vector<int>({6, 8});

  UNIT_ASSERT_EQUAL(2UL, result.size());
  UNIT_ASSERT_TRUE(expected_result == result);
}

void StreamsTest::test_every()
{
  auto result = make_stream(1, 12)
    .every(3)
    .collect<std::vector>();

  auto expected_result = std::vector<int>({3, 6, 9, 12});

  UNIT_ASSERT_EQUAL(4UL, result.size());
  UNIT_ASSERT_TRUE(expected_result == result);
}
