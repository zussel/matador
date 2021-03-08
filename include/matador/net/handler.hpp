#ifndef MATADOR_HANDLER_HPP
#define MATADOR_HANDLER_HPP

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

#include <memory>
#include <string>

namespace matador {

class reactor;

/**
 * Base class for all handlers used
 * with the reactor. The handler must implement its
 * interface to handle input, output, exceptional and
 * timeout data.
 */
class OOS_NET_API handler : public std::enable_shared_from_this<handler>
{
public:
  /**
   * Virtual destructor
   */
  virtual ~handler() = default;

  /**
   * Interface to open a handler. This
   * is called when a handler is registered
   * within the reactor
   */
  virtual void open() = 0;

  /**
   * Interface to returns the socket handle
   * of the concrete handler implementation.
   *
   * @return The socket fd
   */
  virtual int handle() const = 0;

  /**
   * Interface handling incoming data
   */
  virtual void on_input() = 0;

  /**
   * Interface handling outgoing data
   */
  virtual void on_output() = 0;

  /**
   * Interface handling exceptional data
   */
  virtual void on_except() = 0;

  /**
   * Interface handling timout data
   */
  virtual void on_timeout() = 0;

  /**
   * Interface called when the handler is closed
   */
  virtual void on_close() = 0;

  /**
   * Interface implementation should close
   * the handle gracefully
   */
  virtual void close() = 0;

  /**
   * Interface should return true if there
   * is outgoing data
   *
   * @return True if there is outgoing data
   */
  virtual bool is_ready_write() const = 0;

  /**
   * Interface should return true if there
   * is incoming data
   *
   * @return True if there is incoming data
   */
  virtual bool is_ready_read() const = 0;

  /**
   * Returns the next timeout scheduled
   * in the reactor
   *
   * @return Next timeout
   */
  time_t next_timeout() const;

  /**
   * Returns the timeout interval in
   * seconds
   *
   * @return Timeout interval
   */
  time_t interval() const;

  /**
   * Get the name of the handler.
   * The name don't need to be unique.
   * It has only an informational purpose.
   *
   * @return The name of the handler
   */
  virtual std::string name() const = 0;

protected:

  /**
   * Gets the underlying reactor
   *
   * @return The underlying reactor
   */
  reactor* get_reactor() const;

private:
  friend class reactor;

  void register_reactor(reactor *r);
  void schedule(time_t offset, time_t interval);
  void cancel_timer();
  void calculate_next_timeout(time_t now);

  reactor *reactor_ = nullptr;

  time_t next_timeout_ = 0;
  time_t interval_ = 0;
};

}
#endif //MATADOR_HANDLER_HPP
