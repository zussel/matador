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

namespace matador {

class reactor;

class OOS_NET_API handler : public std::enable_shared_from_this<handler>
{
public:
  virtual void open() = 0;
  virtual int handle() const = 0;

  virtual void on_input() = 0;
  virtual void on_output() = 0;
  virtual void on_except() = 0;
  virtual void on_timeout() = 0;
  virtual void on_close() = 0;

  virtual void close() = 0;

  virtual bool is_ready_write() const = 0;
  virtual bool is_ready_read() const = 0;

  time_t next_timeout() const;
  time_t interval() const;

protected:
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
