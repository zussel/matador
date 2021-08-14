#include "StreamsTest.hpp"

#include "matador/utils/stream.hpp"

#include <list>

using namespace matador;

StreamsTest::StreamsTest()
  : unit_test("streams", "streams test")
{
  add_test("generate", [this] { test_generate(); }, "streams generate test");
  add_test("iterate", [this] { test_iterate(); }, "streams iterate test");
  add_test("min", [this] { test_min(); }, "streams find minimum test");
  add_test("max", [this] { test_max(); }, "streams find maximum test");
  add_test("filter", [this] { test_filter(); }, "streams filter elements test");
  add_test("map", [this] { test_map(); }, "streams map elements test");
  add_test("flatmap", [this] { test_flatmap(); }, "streams flatmap elements test");
  add_test("take", [this] { test_take(); }, "streams take elements test");
  add_test("take_while", [this] { test_take_while(); }, "streams take while elements test");
  add_test("skip", [this] { test_skip(); }, "streams skip elements test");
  add_test("skip_while", [this] { test_skip_while(); }, "streams skip while elements test");
  add_test("every", [this] { test_every(); }, "streams every elements test");
  add_test("peek", [this] { test_peek(); }, "streams peek element test");
  add_test("concat", [this] { test_concat(); }, "streams concat stream test");
  add_test("pack_every", [this] { test_pack_every(); }, "streams pack every stream test");
  add_test("first", [this] { test_first(); }, "streams first element test");
  add_test("last", [this] { test_last(); }, "streams last element test");
  add_test("at", [this] { test_at(); }, "streams element at test");
  add_test("any", [this] { test_any(); }, "streams any element test");
  add_test("all", [this] { test_all(); }, "streams all elements test");
  add_test("none", [this] { test_none(); }, "streams none element test");
  add_test("count", [this] { test_count(); }, "streams element count test");
  add_test("reduce", [this] { test_reduce(); }, "streams reduce elements test");
  add_test("join", [this] { test_join(); }, "streams join to string elements test");
}

bool is_even(int val) { return val % 2 == 0; }

namespace testing {

struct person
{
  person(std::string n, int a) : name(std::move(n)), age(a) {}

  person(std::string n, int a, std::initializer_list<std::string> c)
    : name(std::move(n)), age(a), colors(c)
  {}

  std::string name;
  int age = 0;
  std::vector<std::string> colors;
};

}

void StreamsTest::test_generate()
{
  auto result = make_stream<int>({}).collect<std::vector>();

  result = make_stream(3, 17)
    .collect<std::vector>();

  std::vector<int> expected_result = { 3,4,5,6,7,8,9,10,11,12,13,14,15,16,17 };

  UNIT_ASSERT_EQUAL(15UL, result.size());
  UNIT_ASSERT_TRUE(expected_result == result);

  result = make_stream(3, 17, 2)
    .collect<std::vector>();

  expected_result = { 3,5,7,9,11,13,15,17 };

  UNIT_ASSERT_EQUAL(8UL, result.size());
  UNIT_ASSERT_TRUE(expected_result == result);

  result = make_stream_counter(1)
    .take(5)
    .collect<std::vector>();

  expected_result = { 1,2,3,4,5 };

  UNIT_ASSERT_EQUAL(5UL, result.size());
  UNIT_ASSERT_TRUE(expected_result == result);

  result = make_stream_counter(1, 2)
    .take(5)
    .collect<std::vector>();

  expected_result = { 1,3,5,7,9 };

  UNIT_ASSERT_EQUAL(5UL, result.size());
  UNIT_ASSERT_TRUE(expected_result == result);
}

void StreamsTest::test_iterate()
{
  auto s = make_stream({ 1, 2, 3, 4 });

  auto result = s.collect<std::list>();

  UNIT_ASSERT_EQUAL(4UL, result.size());

  s = make_stream(1, 4);

  auto first = s.begin();

  UNIT_ASSERT_EQUAL(1, *first);

  ++first;

  UNIT_ASSERT_EQUAL(2, *first);

  auto current = first++;

  UNIT_ASSERT_EQUAL(2, *current);

  std::vector<std::shared_ptr<std::string>> shared_string_vector {
    std::make_shared<std::string>("hello"),
    std::make_shared<std::string>("world"),
    std::make_shared<std::string>("salut"),
    std::make_shared<std::string>("stream")
  };

  auto res = make_stream(shared_string_vector)
    .filter([](const auto &val){ return *val == "world"; })
    .map([](const auto &val){ return *val; })
    .collect<std::list>();

  UNIT_ASSERT_EQUAL(1UL, res.size());
}

void StreamsTest::test_min()
{
  auto minval = make_stream(1, 8).min();

  UNIT_ASSERT_TRUE(minval.has_value());
  UNIT_ASSERT_EQUAL(1, minval.value());
}

void StreamsTest::test_max()
{
  auto maxval = make_stream({1, 2, 3, 4, 9, 4, 2, 6}).max();

  UNIT_ASSERT_TRUE(maxval.has_value());
  UNIT_ASSERT_EQUAL(9, maxval.value());
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

void StreamsTest::test_flatmap()
{
  auto result = make_stream<::testing::person>({
    {"otto",   34, { "red", "green" }},
    {"jane",   27, { "blue"}},
    {"george", 56, { "brown", "purple" }},
    {"bobby",  15, { "yellow", "gold", "silver" }}
  }).flatmap([](const ::testing::person &p) {
    return p.colors;
  }).collect<std::vector>();

  std::vector<std::string> expected_result = { "red", "green", "blue", "brown", "purple", "yellow", "gold", "silver" };
  UNIT_ASSERT_EQUAL(8UL, result.size());
  UNIT_ASSERT_TRUE(expected_result == result);
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
  auto result = make_stream({1, 2, 3, 4, 5, 4, 3, 2, 1})
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
  auto result = make_stream({1, 2, 2, 3, 4, 5, 6, 5, 4, 3, 2, 1})
    .filter(is_even)
    .skip_while([](const int &i) { return i > 1 && i < 4; })
    .collect<std::list>();

  auto expected_result = std::list<int>({4,6,4,2});

  UNIT_ASSERT_EQUAL(4UL, result.size());
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

void StreamsTest::test_concat()
{
  auto s = make_stream(6, 10);
  auto result = make_stream(1, 5)
    .concat(s)
    .collect<std::vector>();

  std::vector<int> expected_result = { 1,2,3,4,5,6,7,8,9,10 };

  UNIT_ASSERT_EQUAL(10UL, result.size());
  UNIT_ASSERT_TRUE(expected_result == result);
}

void StreamsTest::test_pack_every()
{
  auto result = make_stream(1, 13)
    .pack_every(3)
    .collect<std::vector>();

  std::vector<std::vector<int>> expected_result = { {1,2,3},{4,5,6},{7,8,9},{10,11,12},{13} };

  UNIT_ASSERT_EQUAL(4UL, result.size());
  for (auto i = 0UL; i < result.size(); ++i) {
    UNIT_ASSERT_TRUE(expected_result.at(i) == result.at(i));
  }
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
  UNIT_ASSERT_EQUAL(8, last_value.value());

  last_value = make_stream(1, 4)
    .filter([](const int &val) { return val > 4; })
    .last();

  UNIT_ASSERT_FALSE(last_value.has_value());
}

void StreamsTest::test_at()
{
  auto value_at_index = make_stream(1, 8)
    .at(4);

  UNIT_ASSERT_TRUE(value_at_index.has_value());
  UNIT_ASSERT_EQUAL(5, value_at_index.value());

  value_at_index = make_stream(1, 8)
    .at(9);

  UNIT_ASSERT_FALSE(value_at_index.has_value());
}

void StreamsTest::test_any()
{
  UNIT_ASSERT_TRUE(make_stream(1, 6).any(is_even));
  UNIT_ASSERT_FALSE(make_stream(1, 6).any([](const int &val) { return val > 7; }));
}

void StreamsTest::test_all()
{
  UNIT_ASSERT_TRUE(make_stream(1, 6).all([](const int &val) { return val < 7; }));
  UNIT_ASSERT_FALSE(make_stream(1, 8).all([](const int &val) { return val < 7; }));

}

void StreamsTest::test_none()
{
  UNIT_ASSERT_TRUE(make_stream(8, 11).none([](const int &val) { return val < 7; }));
  UNIT_ASSERT_FALSE(make_stream(1, 8).none([](const int &val) { return val < 7; }));

}

void StreamsTest::test_count()
{
  UNIT_ASSERT_EQUAL(4UL, make_stream(8, 11).count());
  UNIT_ASSERT_EQUAL(20UL, make_stream(-8, 11).count());
  UNIT_ASSERT_EQUAL(10UL, make_stream(-8, 11).count(is_even));
}

void StreamsTest::test_reduce()
{
  auto reduce_result = make_stream(1, 8).reduce(std::plus<>());

  UNIT_ASSERT_TRUE(reduce_result.has_value());
  UNIT_ASSERT_EQUAL(36, reduce_result.value());

  auto reduce_identity_result = make_stream(1, 8).reduce(std::string(), [](const std::string &result, int i) {
    std::stringstream istr;
    if (!result.empty()) {
      istr << ",";
    }
    istr << i;
    return result + istr.str();
  });

  UNIT_ASSERT_EQUAL("1,2,3,4,5,6,7,8", reduce_identity_result);

  reduce_identity_result = make_stream<int>({}).reduce(std::string(), [](const std::string &result, int i) {
    std::stringstream istr;
    if (!result.empty()) {
      istr << ",";
    }
    istr << i;
    return result + istr.str();
  });

  UNIT_ASSERT_TRUE(reduce_identity_result.empty());

  auto reduce_identity_func_result = make_stream(1, 8).reduce_idfunc([](const int &val) {
    std::stringstream istr;
    istr << val;
    return istr.str();
  }, [](const std::string &result, int i) {
    std::stringstream istr;
    istr << " <-> " << i;
    return result + istr.str();
  });

  UNIT_ASSERT_TRUE(reduce_identity_func_result.has_value());
  UNIT_ASSERT_EQUAL("1 <-> 2 <-> 3 <-> 4 <-> 5 <-> 6 <-> 7 <-> 8", reduce_identity_func_result.value());
}

void StreamsTest::test_join()
{
  auto result = make_stream({"one", "two", "three", "four"}).join();

  UNIT_ASSERT_EQUAL("onetwothreefour", result);

  result = make_stream({"one", "two", "three", "four"}).join(", ");

  UNIT_ASSERT_EQUAL("one, two, three, four", result);

  result = make_stream({"one", "two", "three", "four"}).join(", ", "[", "]");

  UNIT_ASSERT_EQUAL("[one, two, three, four]", result);
}
