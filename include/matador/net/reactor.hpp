#ifndef MATADOR_REACTOR_HPP
#define MATADOR_REACTOR_HPP

#include "matador/net/event_type.hpp"
#include "matador/net/select_fdsets.hpp"

#include "matador/logger/logger.hpp"

#include <list>
#include <memory>

namespace matador {

class handler;

class reactor {
public:
  reactor();

  void register_handler(std::shared_ptr<handler> h, event_type type);
  void unregister_handler(std::shared_ptr<handler> h, event_type type);

  void run();
  void shutdown();

  const select_fdsets& fdsets() const;

  void mark_handler_for_delete(const std::shared_ptr<handler>& h);

private:
  void process_handler(int num);
  void process_timeout();

  void on_read_mask();
  void on_write_mask();
  void on_except_mask();

  void prepare_fdsets();

  void cleanup();

private:
  std::shared_ptr<handler> sentinel_;
  std::list<std::shared_ptr<handler>> handlers_;
  std::list<std::shared_ptr<handler>> handlers_to_delete_;

  select_fdsets fdsets_;

  bool running_ = false;

  logger log_;
};

}

#endif //MATADOR_REACTOR_HPP
