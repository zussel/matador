#include <catch2/catch_test_macros.hpp>

#include "matador/utils/message_bus.hpp"

#include <thread>

struct TestEvent { int value; };
struct FilterEvent { int value; };
struct ClassEvent { std::string msg; };

class Receiver {
public:
    void on_event(const ClassEvent& e) {
        received.push_back(e.msg);
    }
    std::vector<std::string> received;
};

using namespace matador::utils;

TEST_CASE("MessageBus: Basic publish/subscribe works", "[MessageBus]") {
    message_bus bus;
    int counter = 0;

    auto sub = bus.subscribe<TestEvent>([&](const TestEvent& e) {
        counter += e.value;
    });

    bus.publish(TestEvent{1});
    bus.publish(TestEvent{2});

    REQUIRE(counter == 3);
}

TEST_CASE("MessageBus: Filtering works", "[MessageBus]") {
    message_bus bus;
    int counter = 0;

    auto sub = bus.subscribe<FilterEvent>(
        [&](const FilterEvent& e) { counter += e.value; },
        [](const FilterEvent& e) { return e.value % 2 == 0; } // only even
    );

    bus.publish(FilterEvent{1}); // should be filtered out
    bus.publish(FilterEvent{2});
    bus.publish(FilterEvent{3}); // filtered
    bus.publish(FilterEvent{4});

    REQUIRE(counter == 6); // only 2 + 4
}

TEST_CASE("MessageBus: Member function subscription works", "[MessageBus]") {
    message_bus bus;
    Receiver r;

    auto sub = bus.subscribe<ClassEvent>(&r, &Receiver::on_event);
    bus.publish(ClassEvent{"hello"});
    bus.publish(ClassEvent{"world"});

    REQUIRE(r.received.size() == 2);
    REQUIRE(r.received[0] == "hello");
    REQUIRE(r.received[1] == "world");
}

TEST_CASE("MessageBus: SharedPtr instance subscription works", "[MessageBus]") {
    message_bus bus;
    const auto r = std::make_shared<Receiver>();

    auto sub = bus.subscribe<ClassEvent>(r, &Receiver::on_event);
    bus.publish(ClassEvent{"foo"});

    REQUIRE(r->received.size() == 1);
    REQUIRE(r->received[0] == "foo");
}

TEST_CASE("MessageBus: RAII unsubscription works", "[MessageBus]") {
    message_bus bus;
    int counter = 0;

    {
        auto sub = bus.subscribe<TestEvent>([&](const TestEvent& e) {
            counter += e.value;
        });
        bus.publish(TestEvent{5});
        REQUIRE(counter == 5);
    }

    // sub goes out of scope, unsubscribed
    bus.publish(TestEvent{5});
    REQUIRE(counter == 5);
}

TEST_CASE("MessageBus: Type-erased AnyMessage publish works", "[MessageBus]") {
    message_bus bus;
    int counter = 0;

    auto sub = bus.subscribe<TestEvent>([&](const TestEvent& e) {
        counter += e.value;
    });

    const message msg(TestEvent{10});
    bus.publish(msg);

    REQUIRE(counter == 10);
}

TEST_CASE("MessageBus: Multiple subscribers all receive messages", "[MessageBus]") {
    message_bus bus;
    int a = 0, b = 0;

    auto sub1 = bus.subscribe<TestEvent>([&](const TestEvent& e) { a += e.value; });
    auto sub2 = bus.subscribe<TestEvent>([&](const TestEvent& e) { b += e.value; });

    bus.publish(TestEvent{3});
    REQUIRE(a == 3);
    REQUIRE(b == 3);
}

TEST_CASE("MessageBus: Stress test with multi-threaded publish and subscribe", "[MessageBus][stress]") {
    message_bus bus;
    std::atomic<int> counter{0};
    constexpr int numThreads = 8;
    constexpr int eventsPerThread = 5000;

    // Subscribe before launching publishers
    auto sub = bus.subscribe<TestEvent>([&](const TestEvent& e) {
        counter.fetch_add(e.value, std::memory_order_relaxed);
    });

    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    // Publisher threads
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&] {
            for (int i = 0; i < eventsPerThread; ++i) {
                bus.publish(TestEvent{1});
            }
        });
    }

    // Concurrent subscriber thread
    threads.emplace_back([&] {
        for (int i = 0; i < 1000; ++i) {
            auto tempSub = bus.subscribe<TestEvent>([&](const TestEvent& e) {
                counter.fetch_add(e.value, std::memory_order_relaxed);
            });
            std::this_thread::yield(); // simulate churn
        }
    });

    for (auto& th : threads) {
        th.join();
    }

    REQUIRE(counter.load(std::memory_order_relaxed) >= numThreads * eventsPerThread);
}
