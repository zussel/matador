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
  add_test("take_while", std::bind(&StreamsTest::test_take_while, this), "streams take while elements test");
  add_test("skip", std::bind(&StreamsTest::test_skip, this), "streams skip elements test");
  add_test("skip_while", std::bind(&StreamsTest::test_skip_while, this), "streams skip while elements test");
  add_test("every", std::bind(&StreamsTest::test_every, this), "streams every elements test");
  add_test("peek", std::bind(&StreamsTest::test_peek, this), "streams peek element test");
  add_test("first", std::bind(&StreamsTest::test_first, this), "streams first element test");
  add_test("last", std::bind(&StreamsTest::test_last, this), "streams last element test");
  add_test("at", std::bind(&StreamsTest::test_at, this), "streams element at test");
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

void StreamsTest::test_take_while()
{
  auto result = make_stream(1, 6)
    .filter(is_even)
    .take_while([](const int &i) { return i > 1 && i < 4; })
    .collect<std::list>();

  auto expected_result = std::list<int>({2});

  UNIT_ASSERT_EQUAL(1UL, result.size());
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

void StreamsTest::test_skip_while()
{
  auto result = make_stream(1, 6)
    .filter(is_even)
    .skip_while([](const int &i) { return i > 1 && i < 4; })
    .collect<std::list>();

  auto expected_result = std::list<int>({4,6});

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

void StreamsTest::test_peek()
{
  int result = 0;
  make_stream(3, 9)
    .take(1)
    .peek([&result](const int &val) { result = val; })
    .count();

  UNIT_ASSERT_EQUAL(3, result);
}

void StreamsTest::test_first()
{
  auto first_value = make_stream(1, 8)
    .filter(is_even)
    .first();

  UNIT_ASSERT_TRUE(first_value.has_value());
  UNIT_ASSERT_EQUAL(2, first_value.value());

  first_value = make_stream(1, 8)
    .filter([](const int &val) { return val > 8; })
    .first();

  UNIT_ASSERT_FALSE(first_value.has_value());
}

void StreamsTest::test_last()
{
  auto last_value = make_stream(1, 9)
    .filter(is_even)
    .last();

  UNIT_ASSERT_TRUE(last_value.has_value());
  UNIT_ASSERT_EQUAL(9, last_value.value());

  last_value = make_stream(1, 4)
    .filter([](const int &val) { return val > 4; })
    .last();

  UNIT_ASSERT_FALSE(last_value.has_value());
}

void StreamsTest::test_at()
{
  auto value_at_index = make_stream(1, 8).at(4);

  UNIT_ASSERT_TRUE(value_at_index.has_value());
  UNIT_ASSERT_EQUAL(5, value_at_index.value());

  value_at_index = make_stream(1, 8).at(9);

  UNIT_ASSERT_FALSE(value_at_index.has_value());
}
