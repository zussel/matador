#ifndef MATADOR_REACTOR_HPP
#define MATADOR_REACTOR_HPP

#include "matador/net/event_type.hpp"
#include "matador/net/select_fdsets.hpp"

#include <vector>
#include <memory>

namespace matador {

class handler;

class reactor {
public:
  void register_handler(std::shared_ptr<handler> h, event_type type);
  void unregister_handler(std::shared_ptr<handler> h, event_type type);

  void run();
  void shutdown();

  const select_fdsets& fdsets() const;

private:
  void process_handler(int num);
  void process_timeout();

  void on_read_mask();
  void on_write_mask();
  void on_except_mask();

  void prepare_fdsets();

  void cleanup();

private:
//  std::unordered_map<int, handler*> handler_map_;

  std::vector<std::shared_ptr<handler>> handlers_;

  select_fdsets fdsets_;

  bool running_ = false;
};

}

#endif //MATADOR_REACTOR_HPP
