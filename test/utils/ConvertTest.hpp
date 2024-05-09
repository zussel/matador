#ifndef MATADOR_CONVERTTEST_HPP
#define MATADOR_CONVERTTEST_HPP

#include "matador/unit/unit_test.hpp"

class ConvertTest : public matador::unit_test
{
public:
  ConvertTest();

  void test_convert_integral();
  void test_convert_floating_point();

private:
  template<typename From, typename To>
  void validate_conversion(From from);
  template<typename From, typename To>
  void validate_conversion(From from, To expected_to);
  template<typename From>
  void validate_integral_conversion(From from);
};


#endif //MATADOR_CONVERTTEST_HPP
