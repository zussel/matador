#include "leader_follower_thread_pool.hpp"

#include "matador/utils/thread_helper.hpp"

#include <iostream>
#include <random>

class taxi_station
{
public:
  taxi_station(int customers, std::size_t threads)
    : pool_(threads, [this]() { wait_for_customer(); })
    , customers_(customers)
    , log_(matador::create_logger("TaxiStation"))
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
    while (handled_customers_ < customers_) {
      pool_.promote_new_leader();

      handle_customer();
    }

    log_.info("handled customers == customers (%d == %d)",handled_customers_.operator int(), customers_.operator int());
    no_customer_.notify_one();
  }

  void handle_customer()
  {
    log_.info("handle customer start");
    std::mt19937_64 eng{std::random_device{}()};  // or seed however you want
    std::uniform_int_distribution<> dist{1, 3};
    auto delay = std::chrono::seconds{dist(eng)};
    log_.info("delay %d secs", delay.count());
    std::this_thread::sleep_for(delay);
    log_.info("finished handling customer %d", ++handled_customers_);
  }

private:
  leader_follower_thread_pool pool_;

  std::atomic_int customers_ {};
  std::atomic_int handled_customers_ {};

  std::thread::id nullid {0};

  std::mutex mutex_;
  std::condition_variable no_customer_;

  matador::logger log_;
};


int main()
{

  int num_threads = std::thread::hardware_concurrency();

  matador::add_log_sink(matador::create_stdout_sink());

  taxi_station t1(10, num_threads);

  t1.run();

  return 0;
}
