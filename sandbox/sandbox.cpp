#include "matador/utils/stream.hpp"

#include <string>
#include <iostream>
#include <sstream>

using namespace matador;

bool is_even(int i) { return i % 2 == 0; }
bool is_odd(int i) { return i % 2 != 0; }
std::string to_string(int i) { return typeid(i).name() + std::to_string(i); }

struct person
{
  person(std::string n, int a) : name(std::move(n)), age(a) {}
  std::string name;
  int age = 0;
};

int main()
{
  std::list<int> ints = { 1, 2, 3, 4 };

  auto range = detail::make_from<int>(std::begin(ints), std::end(ints));

  auto filter = make_filter([](const int &in) {
    return in % 2 == 0;
  }, range);

  auto mapper = detail::make_mapper([](const int &in) {
    return typeid(in).name() + std::to_string(in);
  }, filter);

  mapper->process();
  std::cout << "Mapper: " << *mapper->value() << "\n";

  auto s = make_stream(ints);

  auto l = s.collect<std::vector>();

  for (const auto &i : l) {
    std::cout << "i: " << i << "\n";
  }

  std::cout << "done\n";

  auto s2 = s
    .filter(is_even)
    .map(to_string);

  auto result = s2.collect<std::list>();

  for (const auto &i : s2) {
    std::cout << "i: " << i << "\n";
  }

  std::cout << "done\n";


  auto res = collect<std::string, std::vector>(s2);

  auto first_value = make_stream(ints).filter(is_even).first();

  std::cout << "optional first has value: " << std::boolalpha << first_value.has_value() << "\n";
  std::cout << "optional first value: " << first_value.value() << "\n";

  first_value = make_stream(ints).filter([](const int &val) { return val > 4; }).first();

  std::cout << "optional first has value: " << std::boolalpha << first_value.has_value() << "\n";
  try {
    std::cout << "optional first value: " << first_value.value() << "\n";
  } catch (bad_optional_access &ex) {
    std::cout << "caught exception: " << ex.what() << "\n";
  }

  auto value_at_index = make_stream(1, 8).at(4);

  std::cout << "optional at index has value: " << std::boolalpha << value_at_index.has_value() << "\n";
  std::cout << "optional at index value: " << value_at_index.value() << "\n";

  value_at_index = make_stream(1, 8).at(9);

  std::cout << "optional at index has value: " << std::boolalpha << value_at_index.has_value() << "\n";
  try {
    std::cout << "optional at index value: " << value_at_index.value() << "\n";
  } catch (bad_optional_access &ex) {
    std::cout << "caught exception: " << ex.what() << "\n";
  }

  auto last_value = make_stream(ints).filter(is_even).last();

  std::cout << "optional last has value: " << std::boolalpha << last_value.has_value() << "\n";
  std::cout << "optional last value: " << last_value.value() << "\n";

  last_value = make_stream(ints).filter([](const int &val) { return val > 4; }).last();

  std::cout << "optional last has value: " << std::boolalpha << last_value.has_value() << "\n";
  try {
    std::cout << "optional last value: " << last_value.value() << "\n";
  } catch (bad_optional_access &ex) {
    std::cout << "caught exception: " << ex.what() << "\n";
  }

  auto pc = make_stream(ints)
    .peek([](const int &val) { std::cout << val <<  " (peek)\n";})
    .count();

  std::cout << "peek count: " << pc << "\n";

  std::cout << "done\n";

  make_stream(ints)
    .take(2)
    .print_to(std::cout, " (take)\n");

  std::cout << "done\n";

  make_stream(ints)
    .skip(2)
    .print_to(std::cout, " (skip)\n");

  std::cout << "done\n";

  make_stream(ints)
    .skip_while([](const int &i) { return i > 1 && i < 4; })
    .print_to(std::cout, " (skip while)\n");

  std::cout << "done\n";

  make_stream(ints)
    .take_while([](const int &i) { return i > 1 && i < 4; })
    .print_to(std::cout, " (take while)\n");

  std::cout << "done\n";

  std::vector<person> persons = {
    { "otto", 34 },
    { "jane", 27 },
    { "george", 56 },
    { "bobby", 15 }
  };

  make_stream(persons)
    .map([](const person &p) { return p.age; })
    .print_to(std::cout, " (map age)\n");

  std::cout << "done\n";

  make_stream(3, 17)
    .print_to(std::cout, " (range)\n");

  std::cout << "count range(3, 17): " << make_stream(3, 17).count() << "\n";

  make_stream(3, 17, 2)
  .print_to(std::cout, " (range 2)\n");

  std::cout << "count range(3, 17, 2): " << make_stream(3, 17, 2).count() << "\n";

  std::cout << "done\n";

  make_stream_counter(6)
    .filter(is_even)
    .take(10)
    .print_to(std::cout, " (counter)\n");

  std::cout << "done\n";

  make_stream_counter(6, 4)
    .filter(is_even)
    .take(10)
    .print_to(std::cout, " (counter)\n");

  std::cout << "any even in range(1, 6): " << std::boolalpha << make_stream(1, 6).any(is_even) << "\n";
  std::cout << "all less 7 range(1, 6): " << std::boolalpha << make_stream(1, 6).all([](const int &val) { return val < 7; }) << "\n";
  std::cout << "all less 7 range(1, 8): " << std::boolalpha << make_stream(1, 8).all([](const int &val) { return val < 7; }) << "\n";
  std::cout << "none less 7 range(8, 11): " << std::boolalpha << make_stream(8, 11).none([](const int &val) { return val < 7; }) << "\n";
  std::cout << "none less 7 range(1, 8): " << std::boolalpha << make_stream(1, 8).none([](const int &val) { return val < 7; }) << "\n";

  auto reduce_result = make_stream(1, 8).reduce(std::plus<int>());

  std::cout << "reduce(1, 8): " << reduce_result.value() << "\n";

  auto reduce_identity_result = make_stream(1, 8).reduce(std::string(), [](const std::string &result, int i) {
    std::stringstream istr;
    if (!result.empty()) {
      istr << ",";
    }
    istr << i;
    return result + istr.str();
  });

  std::cout << "reduce identity(1,8): " << reduce_identity_result << "\n";

  auto reduce_identity_result_2 = make_stream(1, 8).reduce_idfunc([](const int &val) {
    std::stringstream istr;
    istr << val;
    return istr.str();
  }, [](const std::string &result, int i) {
    std::stringstream istr;
    istr << " <-> " << i;
    return result + istr.str();
  });

  std::cout << "reduce identity(1,8): " << reduce_identity_result_2 << "\n";

  auto minval = make_stream(1, 8).min();

  std::cout << "min (1,8): " << minval.value() << "\n";

  return 0;
}