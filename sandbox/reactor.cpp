#include "matador/net/leader_follower_thread_pool.hpp"

#include "matador/utils/thread_helper.hpp"

#include "matador/logger/log_manager.hpp"

#include <iostream>
#include <random>

class taxi_station
{
public:
  taxi_station(int customers, std::size_t threads)
    : pool_(threads, [this]() { wait_for_customer(); })
    , customers_(customers)
    , log_(matador::create_logger("TaxiStation"))
    , eng_{std::random_device{}()}
  {}

  void run()
  {
    log_.info("start processing all %d customers", customers_.operator int());
    pool_.start();

    std::unique_lock<std::mutex> l(mutex_);
    no_customer_.wait(l, [this]() {
      return handled_customers_ >= customers_;
    });
    log_.info("all customers processed %d", customers_.operator int());
    pool_.shutdown();
  }

private:
  void wait_for_customer()
  {
    if (handled_customers_ == 10) {
      log_.info("handled customers == customers (%d == %d)",handled_customers_.operator int(), customers_.operator int());
      no_customer_.notify_one();
    }
    log_.info("waiting for customer %d", handled_customers_.operator int());
    std::uniform_int_distribution<> dist{1, 3};
    auto delay = std::chrono::milliseconds {dist(eng_) * 100};
    log_.info("delay %d millis", delay.count());
    std::this_thread::sleep_for(delay);

    pool_.promote_new_leader();

    handle_customer(handled_customers_++);

  }

  void handle_customer(int customer)
  {
    log_.info("handle customer %d start", customer);
    std::uniform_int_distribution<> dist{1, 3};
    auto delay = std::chrono::seconds{dist(eng_)};
    log_.info("delay %d secs", delay.count());
    std::this_thread::sleep_for(delay);
    log_.info("finished handling customer %d", customer);
  }

private:
  matador::leader_follower_thread_pool pool_;

  std::atomic_int customers_ {};
  std::atomic_int handled_customers_ {};

  std::thread::id nullid {};

  std::mutex mutex_;
  std::condition_variable no_customer_;

  matador::logger log_;

  std::mt19937_64 eng_;
};


int main()
{
  auto num_threads = std::thread::hardware_concurrency();

  matador::add_log_sink(matador::create_stdout_sink());

  taxi_station t1(10, num_threads);

  t1.run();

  return 0;
}
