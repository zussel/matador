#include <catch2/catch_test_macros.hpp>

#include "matador/utils/di.hpp"

#include <atomic>
#include <iostream>
#include <mutex>
#include <set>
#include <thread>
#include <vector>

namespace detail {
namespace {
class greeter {
public:
  virtual ~greeter() = default;

  [[nodiscard]] virtual std::string greet() const = 0;
};

class smart_greeter final : public greeter {
public:
  [[nodiscard]] std::string greet() const override { return "hey dude"; }
};

class hello_greeter final : public greeter {
public:
  [[nodiscard]] std::string greet() const override { return "hello"; }
};

class vehicle
{
public:
  virtual ~vehicle() = default;

  [[nodiscard]] virtual long id() const = 0;
};

class truck final : public vehicle
{
public:
  truck() : id_(++id_counter_) {}

  [[nodiscard]] long id() const override { return id_; }

private:
  static long id_counter_;
  long id_{};
};

long truck::id_counter_ = 0;

class unknown {};

class per_thread {
public:
  virtual ~per_thread() = default;
  virtual void dump() = 0;
  [[nodiscard]] virtual std::thread::id owner() const = 0;
};

class per_thread_dumper final : public per_thread
{
public:
  explicit per_thread_dumper(std::string name)
  : name_(std::move(name))
  , owner_(std::this_thread::get_id())
  {}

  void dump() override {
    std::cout << name_ << ": thread id " << std::this_thread::get_id() << "\n";
  }

  [[nodiscard]] std::thread::id owner() const override {
    return owner_;
  }

private:
  std::string name_;
  std::thread::id owner_;
};

class externally_configured_service {
public:
  virtual ~externally_configured_service() = default;
  [[nodiscard]] virtual int value() const = 0;
  virtual void value(int v) = 0;
};

class externally_configured_service_impl final : public externally_configured_service {
public:
  explicit externally_configured_service_impl(int v)
  : value_(v)
  {}

  [[nodiscard]] int value() const override {
    return value_;
  }

  void value(const int v) override {
    value_ = v;
  }

private:
  int value_;
};

class left_base {
public:
  virtual ~left_base() = default;
  [[nodiscard]] virtual int left() const = 0;
};

class right_interface {
public:
  virtual ~right_interface() = default;
  [[nodiscard]] virtual int right() const = 0;
};

class multiple_inheritance_service final : public left_base, public right_interface {
public:
  [[nodiscard]] int left() const override {
    return 1;
  }

  [[nodiscard]] int right() const override {
    return 2;
  }
};

class counted_service {
public:
  virtual ~counted_service() = default;
  [[nodiscard]] virtual int marker() const = 0;
};

class counted_service_impl final : public counted_service {
public:
  counted_service_impl()
  : marker_(++constructed)
  {}

  [[nodiscard]] int marker() const override {
    return marker_;
  }

  static std::atomic<int> constructed;

private:
  int marker_;
};

class reload_service {
public:
  virtual ~reload_service() = default;
  virtual std::string name() const = 0;
};

class reload_service_a final : public reload_service {
public:
  explicit reload_service_a(std::shared_ptr<std::atomic<int>> destroyed)
  : destroyed_(std::move(destroyed))
  {}

  ~reload_service_a() override {
    ++(*destroyed_);
  }

  std::string name() const override {
    return "a";
  }

private:
  std::shared_ptr<std::atomic<int>> destroyed_;
};

class reload_service_b final : public reload_service {
public:
  std::string name() const override {
    return "b";
  }
};

std::atomic<int> counted_service_impl::constructed{0};
}
}

using namespace matador::utils;

TEST_CASE("Test dependency injection", "[di]") {
  di::install_module([](di::module &module) {
    module.bind<detail::greeter>()->to_singleton<detail::smart_greeter>();
  });

  di::inject<detail::greeter> g1;

  REQUIRE(g1);

  auto g2 = g1;

  REQUIRE(g2);
  REQUIRE(g1 == g2);

  g2 = std::move(g1);

  REQUIRE(!g1);
  REQUIRE(g2);
  REQUIRE(g1 != g2);

  auto g3(std::move(g2));

  REQUIRE(!g2);
  REQUIRE(g3);
  REQUIRE(g3 != g2);

  di::module m;
  m.bind<detail::greeter>()->to_singleton<detail::hello_greeter>();
  m.bind<detail::greeter>("smart")->to_singleton<detail::smart_greeter>();

  di::inject<detail::greeter> g4(m, "smart");
  REQUIRE(g4);
  REQUIRE(g4->greet() == "hey dude");

  di::inject<detail::greeter> g5(m);
  REQUIRE(g5);
  REQUIRE(g5->greet() == "hello");

  REQUIRE(g4 != g5);
}

TEST_CASE("Dependency injection transient strategy creates a new instance on each", "[di]") {
  // Dependency injection transient strategy creates a new instance on each resolve
  di::module m;
  m.bind<detail::vehicle>()->to<detail::truck>();

  auto first = m.resolve<detail::vehicle>();
  auto second = m.resolve<detail::vehicle>();

  REQUIRE(first != nullptr);
  REQUIRE(second != nullptr);
  REQUIRE(first != second);
  REQUIRE(first->id() != second->id());
}

TEST_CASE("Dependency injection singleton strategy returns the same instance", "[di]") {
  di::module m;
  m.bind<detail::vehicle>()->to_singleton<detail::truck>();

  auto first = m.resolve<detail::vehicle>();
  auto second = m.resolve<detail::vehicle>();

  REQUIRE(first != nullptr);
  REQUIRE(second != nullptr);
  REQUIRE(first == second);
  REQUIRE(first->id() == second->id());
}

TEST_CASE("Dependency injection named bindings are isolated", "[di]") {
  di::module m;

  m.bind<detail::greeter>("hello")->to_singleton<detail::hello_greeter>();
  m.bind<detail::greeter>("smart")->to_singleton<detail::smart_greeter>();

  auto hello = m.resolve<detail::greeter>("hello");
  auto smart = m.resolve<detail::greeter>("smart");

  REQUIRE(hello != nullptr);
  REQUIRE(smart != nullptr);
  REQUIRE(hello != smart);
  REQUIRE(hello->greet() == "hello");
  REQUIRE(smart->greet() == "hey dude");
}

TEST_CASE("Dependency injection instance strategy references external instance", "[di]") {
  di::module m;

  detail::externally_configured_service_impl service{7};
  m.bind<detail::externally_configured_service>()->to_instance(service);

  auto resolved = m.resolve<detail::externally_configured_service>();

  REQUIRE(resolved != nullptr);
  REQUIRE(resolved.get() == &service);
  REQUIRE(resolved->value() == 7);

  service.value(42);

  REQUIRE(resolved->value() == 42);
}

TEST_CASE("Dependency injection resolves interfaces with multiple inheritance correctly", "[di]") {
  di::module m;

  m.bind<detail::right_interface>()->to_singleton<detail::multiple_inheritance_service>();

  auto resolved = m.resolve<detail::right_interface>();

  REQUIRE(resolved != nullptr);
  REQUIRE(resolved->right() == 2);
}

TEST_CASE("Dependency injection throws for unknown unnamed type", "[di]") {
  di::module m;

  REQUIRE_THROWS_AS(m.resolve<detail::unknown>(), std::logic_error);
}

TEST_CASE("Dependency injection throws for unknown named module", "[di]") {
  di::module m;

  REQUIRE_THROWS_AS(m.resolve<detail::greeter>("missing"), std::logic_error);
}

TEST_CASE("Dependency injection throws when binding exists without strategy", "[di]") {
  di::module m;

  m.bind<detail::greeter>();

  REQUIRE_THROWS_AS(m.resolve<detail::greeter>(), std::logic_error);
}

TEST_CASE("Dependency injection singleton strategy constructs once under concurrent", "[di]") {
  // Dependency injection singleton strategy constructs once under concurrent resolve
  detail::counted_service_impl::constructed = 0;

  di::module m;
  m.bind<detail::counted_service>()->to_singleton<detail::counted_service_impl>();

  std::vector<std::thread> threads;
  std::vector<std::shared_ptr<detail::counted_service>> resolved;
  std::mutex resolved_mutex;

  for (int i = 0; i < 16; ++i) {
    threads.emplace_back([&m, &resolved, &resolved_mutex]() {
      const auto service = m.resolve<detail::counted_service>();

      std::lock_guard lock(resolved_mutex);
      resolved.push_back(service);
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }

  REQUIRE(resolved.size() == 16);
  REQUIRE(detail::counted_service_impl::constructed == 1);

  const auto first = resolved.front();
  for (const auto& service : resolved) {
    REQUIRE(service == first);
    REQUIRE(service->marker() == 1);
  }
}

TEST_CASE("Dependency injection singleton per thread strategy creates one instance per", "[di]") {
  // Dependency injection singleton per thread strategy creates one instance per thread
  di::module m;
  m.bind<detail::per_thread>()->to_singleton_per_thread<detail::per_thread_dumper>("worker");

  std::vector<std::thread> threads;
  std::vector<std::shared_ptr<detail::per_thread>> resolved;
  std::mutex resolved_mutex;

  for (int i = 0; i < 8; ++i) {
    threads.emplace_back([&m, &resolved, &resolved_mutex]() {
      auto first = m.resolve<detail::per_thread>();
      auto second = m.resolve<detail::per_thread>();

      REQUIRE(first == second);
      REQUIRE(first->owner() == std::this_thread::get_id());

      std::lock_guard<std::mutex> lock(resolved_mutex);
      resolved.push_back(first);
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }

  REQUIRE(resolved.size() == 8);

  std::set unique_instances(resolved.begin(), resolved.end());

  REQUIRE(unique_instances.size() == resolved.size());
}

TEST_CASE("Dependency injection keeps previously resolved singleton alive after module", "[di]") {
  // Dependency injection keeps previously resolved singleton alive after module reload
  auto destroyed = std::make_shared<std::atomic<int>>(0);

  matador::utils::di::install_module([destroyed](matador::utils::di::module &module) {
    module.bind<detail::reload_service>()->to_singleton<detail::reload_service_a>(destroyed);
  });

  matador::utils::di::inject<detail::reload_service> old_service;

  REQUIRE(old_service);
  REQUIRE(old_service->name() == "a");

  matador::utils::di::install_module([](matador::utils::di::module &module) {
    module.bind<detail::reload_service>()->to_singleton<detail::reload_service_b>();
  });

  REQUIRE((*destroyed) == 0);
  REQUIRE(old_service);
  REQUIRE(old_service->name() == "a");

  matador::utils::di::inject<detail::reload_service> new_service;

  REQUIRE(new_service);
  REQUIRE(new_service->name() == "b");
}

TEST_CASE("Dependency injection releases old singleton after last shared owner is gone", "[di]") {
  auto destroyed = std::make_shared<std::atomic<int>>(0);

  {
    matador::utils::di::install_module([destroyed](matador::utils::di::module &module) {
      module.bind<detail::reload_service>()->to_singleton<detail::reload_service_a>(destroyed);
    });

    matador::utils::di::inject<detail::reload_service> old_service;

    matador::utils::di::install_module([](matador::utils::di::module &module) {
      module.bind<detail::reload_service>()->to_singleton<detail::reload_service_b>();
    });

    REQUIRE((*destroyed) == 0);
    REQUIRE(old_service->name() == "a");
  }

  REQUIRE((*destroyed) == 1);
}