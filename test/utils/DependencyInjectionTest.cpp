#include "DependencyInjectionTest.hpp"

namespace detail {

class greeter
{
public:
  virtual ~greeter() = default;

  virtual std::string greet() const = 0;
};

class smart_greeter : public greeter
{
public:
  std::string greet() const override
  {
    return "hey dude";
  }
};

class hello_greeter : public greeter
{
public:
  std::string greet() const override
  {
    return "hello";
  }
};

class vehicle
{
public:
  virtual ~vehicle() = default;

  virtual long id() const = 0;
};

class truck : public vehicle
{
public:
  truck() : id_(++id_counter_)
  {}

  long id() const override
  {
    return id_;
  }

private:
  static long id_counter_;
  long id_{};
};

long truck::id_counter_ = 0;

}

DependencyInjectionTest::DependencyInjectionTest()
  : unit_test("di", "dependency injection test")
{

}
