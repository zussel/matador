#ifndef MATADOR_REACTOR_HPP
#define MATADOR_REACTOR_HPP

#ifdef _MSC_VER
#ifdef matador_net_EXPORTS
    #define OOS_NET_API __declspec(dllexport)
    #define EXPIMP_NET_TEMPLATE
  #else
    #define OOS_NET_API __declspec(dllimport)
    #define EXPIMP_NET_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define OOS_NET_API
#endif

#include "matador/net/event_type.hpp"
#include "matador/net/select_fdsets.hpp"
#include "matador/net/socket_interrupter.hpp"

#include "matador/logger/logger.hpp"

#include <list>
#include <memory>
#include <atomic>

namespace matador {

class handler;

/**
 * @brief Implementation of the reactor pattern
 *
 * This class implements the reactor pattern.
 * It is used to dispatch network connection based
 * on file descriptors.
 *
 * Each connection is represented and handled by
 * an instance of a class based on class handler.
 *
 * A handler must be registered within the reactor
 * for a specific event type. The types are
 *
 * - NONE do nothing
 * - READ handle incoming data
 * - WRITE handle outgoing data
 * - EXCEPT handle exceptional data
 * - ACCEPT handle incoming connections
 *
 * The type can be changed while the reactor is running.
 *
 * There is also an acceptor class to handle incoming
 * connections and a connector class to connect actively
 * to a remote service.
 *
 * It's also possible to handle timeouts on a regularly base.
 */
class OOS_NET_API reactor {
public:
  /**
   * Default constructor
   */
  reactor();

  /**
   * Registers a new handler with the reactor for
   * the given event type.
   *
   * @param h Handler to register
   * @param type Event type which the handler is used for
   */
  void register_handler(const std::shared_ptr<handler>& h, event_type type);

  /**
   * Unregisters a handler from the reactor for
   * the given event mask. If the mask id set to NONE and
   * the handle isn't scheduled for a timeout the handler is removed.
   *
   * @param h Handler to unregister.
   * @param type Event mask for which the handler is to be removed
   */
  void unregister_handler(const std::shared_ptr<handler>& h, event_type type);

  void schedule_timer(const std::shared_ptr<handler>& h, time_t offset, time_t interval);
  void cancel_timer(const std::shared_ptr<handler>& h);

  void run();
  void shutdown();

  bool is_running() const;

  const select_fdsets& fdsets() const;

  void mark_handler_for_delete(const std::shared_ptr<handler>& h);

private:
  void process_handler(int num);

  void on_read_mask(const std::shared_ptr<handler>& h);
  void on_write_mask(const std::shared_ptr<handler>& h);
  void on_except_mask(const std::shared_ptr<handler>& h);
  void on_timeout(const std::shared_ptr<handler> &h, time_t i);

  void prepare_select_bits(time_t& timeout);

  void remove_deleted();

  void cleanup();

  int select(struct timeval* timeout);

  bool is_interrupted();

private:
  typedef std::pair<std::shared_ptr<handler>, event_type> t_handler_type;
  std::shared_ptr<handler> sentinel_;
  std::list<t_handler_type> handlers_;
  std::list<std::shared_ptr<handler>> handlers_to_delete_;

  select_fdsets fdsets_;

  std::atomic<bool> running_ {false};
  std::atomic<bool> shutdown_requested_ {false};

  logger log_;

  socket_interrupter interrupter_;
};

}

#endif //MATADOR_REACTOR_HPP
