#ifndef MATADOR_HANDLER_HPP
#define MATADOR_HANDLER_HPP

namespace matador {

class reactor;

class handler
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

private:
  friend class reactor;

  void register_reactor(reactor *r);

  reactor *reactor_ = nullptr;
};

}
#endif //MATADOR_HANDLER_HPP
