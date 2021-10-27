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
 * connections and a connector class to execute actively
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

  ~reactor();
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

  /**
   * Schedules a timer handler within the reactor.
   * The handler is registered with a given offset and
   * a given interval.
   * If the interval is zero the timer is only
   * schedules once.
   *
   * @param h Handler to schedule.
   * @param offset Offset for first schedule in seconds.
   * @param interval Interval of the schedule.
   */
  void schedule_timer(const std::shared_ptr<handler>& h, time_t offset, time_t interval);

  /**
   * Cancels all timer for a given handler.
   *
   * @param h Handler to cancel the timer for
   */
  void cancel_timer(const std::shared_ptr<handler>& h);

  /**
   * Starts the dispatching process of the
   * reactor. The call to this method only
   * terminates if the handler list is empty
   * and there are no timers scheduled or
   * shutdown ist called.
   */
  void run();

  /**
   * Shutdown the dispatching process of the
   * reactor.
   */
  void shutdown();

  /**
   * Return true if reactor was started and run was called.
   *
   * @return True if reactor is running
   */
  bool is_running() const;

  /**
   * Returns the internal fd sets for
   * reading, writing and exceptions.
   *
   * @return The internal select fd sets
   */
  const select_fdsets& fdsets() const;

  /**
   * Marks the given handler to be deleted
   * from the reactor. That means the handler
   * was closed.
   *
   * @param h Handler to be marked for deletion
   */
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
