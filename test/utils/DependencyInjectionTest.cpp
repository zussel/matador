#include "DependencyInjectionTest.hpp"

#include "matador/utils/di.hpp"

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

class unknown {};

class per_thread
{
public:
  virtual ~per_thread() = default;
  virtual void dump() = 0;
};

class per_thread_dumper : public per_thread
{
public:
  explicit per_thread_dumper(std::string name )
  : name_(std::move(name))
  {}
  void dump() override
  {
    std::cout << name_ << ": thread id " << std::this_thread::get_id() << "\n";
  }

private:
  std::string name_;
};

}

DependencyInjectionTest::DependencyInjectionTest()
  : unit_test("di", "dependency injection test")
{
  add_test("basic", [this] { test_basic(); }, "test di basic behaviour");
  add_test("named", [this] { test_named(); }, "test di named behaviour");
  add_test("transient", [this] { test_transient(); }, "test di transient");
  add_test("singleton", [this] { test_singleton(); }, "test di singleton");
  add_test("instance", [this] { test_instance(); }, "test di instance");
  add_test("singleton_per_thread", [this] { test_singleton_per_thread(); }, "test di singleton pre thread");
}

void DependencyInjectionTest::test_basic()
{
  matador::di::install_module([](matador::di::module &module) {
    module.bind<detail::greeter>()->to_singleton<detail::smart_greeter>();
  });

  matador::di::inject<detail::greeter> g1;

  UNIT_ASSERT_NOT_NULL(g1.get());

  auto g2 = g1;

  UNIT_ASSERT_NOT_NULL(g2.get());
  UNIT_ASSERT_EQUAL(g1.get(), g2.get());

  g2 = std::move(g1);

  UNIT_ASSERT_NULL(g1.get());
  UNIT_ASSERT_NOT_NULL(g2.get());
  UNIT_ASSERT_NOT_EQUAL(g1.get(), g2.get());

  auto g3(std::move(g2));

  UNIT_ASSERT_NULL(g2.get());
  UNIT_ASSERT_NOT_NULL(g3.get());
  UNIT_ASSERT_NOT_EQUAL(g3.get(), g2.get());

  matador::di::module m;
  m.bind<detail::greeter>()->to_singleton<detail::hello_greeter>();
  m.bind<detail::greeter>("smart")->to_singleton<detail::smart_greeter>();

  matador::di::inject<detail::greeter> g4(m, "smart");
  UNIT_ASSERT_NOT_NULL(g4.get());

  matador::di::inject<detail::greeter> g5(m);
  UNIT_ASSERT_NOT_NULL(g5.get());

  UNIT_ASSERT_NOT_EQUAL(g4.get(), g5.get());

  UNIT_ASSERT_EXCEPTION(matador::di::inject<detail::unknown> u, std::logic_error, "unknown type");
}

void DependencyInjectionTest::test_named()
{
  matador::di::install_module([](matador::di::module &module) {
    module.bind<detail::greeter>("smart")->to_singleton<detail::smart_greeter>();
  });

  UNIT_ASSERT_EXCEPTION(matador::di::inject<detail::greeter>("hello"), std::logic_error, "unknown name hello");

  matador::di::append_module([](matador::di::module &module) {
    module.bind<detail::greeter>("hello")->to_singleton<detail::hello_greeter>();
  });


  matador::di::inject<detail::greeter> g1("smart");
  matador::di::inject<detail::greeter> g2("hello");

  UNIT_ASSERT_NOT_NULL(g1.get());
  UNIT_ASSERT_NOT_NULL(g2.get());

  UNIT_ASSERT_NOT_EQUAL(g1.get(), g2.get());
  UNIT_ASSERT_EQUAL("hey dude", g1->greet());
  UNIT_ASSERT_EQUAL("hello", g2->greet());
}

void DependencyInjectionTest::test_transient()
{
  matador::di::install_module([](matador::di::module &m) {
    m.bind<detail::vehicle>()->to<detail::truck>();
  });

  matador::di::inject<detail::vehicle> t1;
  matador::di::inject<detail::vehicle> t2;

  UNIT_ASSERT_NOT_NULL(t1.get());
  UNIT_ASSERT_NOT_NULL(t2.get());

  UNIT_ASSERT_NOT_EQUAL(t1.get(), t2.get());
}

void DependencyInjectionTest::test_singleton()
{
  matador::di::install_module([](matador::di::module &module) {
    module.bind<detail::greeter>()->to_singleton<detail::smart_greeter>();
  });

  matador::di::inject<detail::greeter> g1;
  matador::di::inject<detail::greeter> g2;

  UNIT_ASSERT_NOT_NULL(g1.get());
  UNIT_ASSERT_NOT_NULL(g2.get());

  UNIT_ASSERT_EQUAL(g1.get(), g2.get());

  matador::di::clear_module();

  UNIT_ASSERT_EXCEPTION(matador::di::inject<detail::greeter>(), std::logic_error, "unknown type");
}

void DependencyInjectionTest::test_instance()
{
  matador::di::install_module([](matador::di::module &m) {
    detail::truck t;
    m.bind<detail::vehicle>()->to_instance(t);
  });

  matador::di::inject<detail::vehicle> t1;
  matador::di::inject<detail::vehicle> t2;

  UNIT_ASSERT_NOT_NULL(t1.get());
  UNIT_ASSERT_NOT_NULL(t2.get());

  UNIT_ASSERT_EQUAL(t1.get(), t2.get());
}

void DependencyInjectionTest::test_singleton_per_thread()
{
  matador::di::install_module([](matador::di::module &module) {
    module.bind<detail::per_thread>()->to_singleton_per_thread<detail::per_thread_dumper>("otto");
  });

  matador::di::inject<detail::per_thread> ptd1;
  matador::di::inject<detail::per_thread> ptd2;

  UNIT_ASSERT_NOT_NULL(ptd1.get());
  UNIT_ASSERT_NOT_NULL(ptd2.get());

  UNIT_ASSERT_EQUAL(ptd1.get(), ptd2.get());

  auto t1 = std::thread([this, ptd1]() {
    matador::di::inject<detail::per_thread> ptd3;
    matador::di::inject<detail::per_thread> ptd4;

    UNIT_ASSERT_NOT_NULL(ptd3.get());
    UNIT_ASSERT_NOT_NULL(ptd4.get());

    UNIT_ASSERT_EQUAL(ptd3.get(), ptd4.get());
    UNIT_ASSERT_NOT_EQUAL(ptd1.get(), ptd4.get());
  });
  t1.join();
}
