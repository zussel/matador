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

  auto s = make_stream(ints);

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

  make_stream(1, 12).every(3).print_to(std::cout, " (every 3)\n");
  return 0;
}