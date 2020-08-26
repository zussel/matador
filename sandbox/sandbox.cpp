#include "matador/utils/buffer.hpp"
#include "matador/utils/os.hpp"
#include "matador/net/acceptor.hpp"
#include "matador/net/connector.hpp"
#include "matador/net/socket.hpp"
#include "matador/net/ip.hpp"
#include "matador/net/reactor.hpp"

#include "matador/logger/logger.hpp"
#include "matador/logger/log_manager.hpp"

#include <utility>
#include <iostream>

using namespace matador;

class echo_server_handler : public handler
{
public:
  echo_server_handler(tcp::socket sock, const tcp::peer& endpoint, acceptor *accptr);

  void open() override;

  int handle() const override;

  void on_input() override;
  void on_output() override;
  void on_except() override {}
  void on_timeout() override;
  void on_close() override;
  void close() override;

  bool is_ready_write() const override;
  bool is_ready_read() const override;

private:
  tcp::socket stream_;
  tcp::peer endpoint_;
  acceptor *acceptor_ = nullptr;

  std::string data_;
  logger log_;
};

class echo_client_handler : public handler
{
public:
  echo_client_handler(tcp::socket sock, const tcp::peer& endpoint, connector *cnnctr);

  void open() override;

  int handle() const override;

  void on_input() override;
  void on_output() override;
  void on_except() override {}
  void on_timeout() override {}
  void on_close() override;
  void close() override;

  bool is_ready_write() const override;
  bool is_ready_read() const override;

private:
  tcp::socket stream_;
  tcp::peer endpoint_;
  connector *connector_ = nullptr;

  std::string data_;
  logger log_;

  std::string message_;
};

void start_client(unsigned short port);
void start_server(unsigned short port);

int main(int argc, char* argv[])
{
  if (argc < 3 || (strcmp("server", argv[1]) != 0 && strcmp("client", argv[1]) != 0)) {
    std::cout << "usage: sandbox [server|client] [port]\n";
    return 1;
  } else {
    unsigned short port = 0;
    try {
      port = std::stoi(argv[2]);
    } catch (std::exception&) {
      std::cout << "usage: sandbox [server|client] [port]\n";
      return 1;
    }


    std::string type = argv[1];

    if (type == "client") {
      start_client(port);
    } else {
      start_server(port);
    }
  }

//  http::server serv;
//
//  serv.on_get("/", [](http::request &request) {
//    return http::response;
//  });
//
//  serv.listen(7090);
}

void start_client(unsigned short port)
{
  net::init();

  matador::add_log_sink(matador::create_file_sink("log/client.log"));
  matador::add_log_sink(matador::create_stdout_sink());

  auto echo_connector = std::make_shared<connector>([](const tcp::socket& sock, const tcp::peer &p, connector *cnnctr) {
    return std::make_shared<echo_client_handler>(sock, p, cnnctr);
  });

  reactor r;

  tcp::resolver resolver;
  auto endpoints = resolver.resolve("localhost", std::to_string(port));
  echo_connector->connect(r, endpoints);

  r.run();

//  tcp::socket s;
//  connect(s, "localhost", port);
//  s.non_blocking(false);
//
//  std::string message;
//  while (true) {
//    std::cout << "Message: ";
//    std::cin >> message;
//    if (message == "exit") {
//      s.close();
//      break;
//    }
//    char buf[16384];
//    buffer chunk(buf, 16384);
//
//    chunk.append(message.c_str(), message.size());
//    s.send(chunk);
//    chunk.reset();
//
//    int ret = s.receive(chunk);
//    message.assign(chunk.data(), ret);
//    chunk.reset();
//
//    std::cout << "Answer: " << message << "\n";
//  }
  net::cleanup();
}

void start_server(unsigned short port)
{
  net::init();

  matador::add_log_sink(matador::create_file_sink("log/server.log"));
  matador::add_log_sink(matador::create_stdout_sink());

  tcp::peer endpoint(address::v4::any() , port);

  auto echo_acceptor = std::make_shared<acceptor>(endpoint, [](const tcp::socket& sock, const tcp::peer &p, acceptor *accptr) {
    return std::make_shared<echo_server_handler>(sock, p, accptr);
  });

  reactor r;
  r.register_handler(echo_acceptor, event_type::ACCEPT_MASK);

  r.run();

  net::cleanup();
}

echo_server_handler::echo_server_handler(tcp::socket sock, const tcp::peer& endpoint, acceptor *accptr)
  : stream_(std::move(sock))
  , endpoint_(endpoint)
  , acceptor_(accptr)
  , log_(create_logger("EchoServerHandler"))
{

}

void echo_server_handler::open()
{

}

int echo_server_handler::handle() const
{
  return stream_.id();
}

void echo_server_handler::on_input()
{
    /*
    GET / HTTP/1.1
    Host: localhost:7090
    User-Agent: curl/7.70.0
    Accept: * / *
    */
  char buf[16384];
  buffer chunk(buf, 16384);
  auto len = stream_.receive(chunk);
  if (len == 0) {
    on_close();
  } else if (len < 0 && errno != EWOULDBLOCK) {
    log_.error("fd %d: error on read: %s", handle(), strerror(errno));
    on_close();
  } else {
    log_.info("received %d bytes", len);
    data_.assign(buf, len);
    log_.info("received data: %s", data_.c_str());
    log_.info("end of data");
  }
}

void echo_server_handler::on_output()
{
  std::string ret = R"(HTTP/1.1 200 OK
Server: Matador/0.7.0
Content-Length: 111
Content-Language: de
Connection: close
Content-Type: text/html

<!doctype html>
<html>
  <head>
    <title>Dummy!</title>
  </head>
  <body>
    <p>Help!</p>
  </body>
</html>
)";

  char buf[16384];
  buffer chunk(buf, 16384);

  chunk.append(data_.c_str(), data_.size());
//  chunk.append(ret.c_str(), ret.size());
  auto len = stream_.send(chunk);
  log_.info("sent %d bytes", len);
  data_.clear();
}

void echo_server_handler::on_timeout()
{
  log_.info("hello from the timeout");
}

void echo_server_handler::on_close()
{
  log_.info("fd %d: closing connection", handle());
  stream_.close();
  auto self = shared_from_this();
  get_reactor()->mark_handler_for_delete(self);
  get_reactor()->unregister_handler(self, event_type::READ_WRITE_MASK);
}

void echo_server_handler::close()
{
    log_.info("fd %d: closing connection", handle());
    stream_.close();
}

bool echo_server_handler::is_ready_write() const
{
  return !data_.empty();
}

bool echo_server_handler::is_ready_read() const
{
  return data_.empty();
}

echo_client_handler::echo_client_handler(tcp::socket sock, const tcp::peer &endpoint, connector *cnnctr)
  : stream_(std::move(sock))
  , endpoint_(endpoint)
  , connector_(cnnctr)
  , log_(create_logger("EchoClientHandler"))
{

}

void echo_client_handler::open()
{

}

int echo_client_handler::handle() const
{
  return stream_.id();
}

void echo_client_handler::on_input()
{
  char buf[16384];
  buffer chunk(buf, 16384);
  int ret = stream_.receive(chunk);
  message_.clear();
  message_.assign(chunk.data(), ret);
  chunk.reset();

  std::cout << "Answer: " << message_ << "\n";
  message_.clear();
}

void echo_client_handler::on_output()
{
  std::cout << "Message: ";
  std::cin >> message_;
  if (message_ == "exit") {
    close();
    return;
  }
  char buf[16384];
  buffer chunk(buf, 16384);

  chunk.append(message_.c_str(), message_.size());
  size_t len = stream_.send(chunk);
  log_.info("sent %d bytes", len);
  chunk.reset();
}

void echo_client_handler::on_close()
{
  log_.info("fd %d: closing connection", handle());
  stream_.close();
  auto self = shared_from_this();
  get_reactor()->mark_handler_for_delete(self);
  get_reactor()->unregister_handler(self, event_type::READ_WRITE_MASK);
}

void echo_client_handler::close()
{
  log_.info("fd %d: closing connection", handle());
  stream_.close();
}

bool echo_client_handler::is_ready_write() const
{
  return message_.empty();
}

bool echo_client_handler::is_ready_read() const
{
  return !message_.empty();
}

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <list>
#include <type_traits>
#include <memory>

template < class T >
class stream_source
{
public:

};

template < class Out  >
class stream_element_processor
{
public:
  virtual ~stream_element_processor() = default;
  virtual std::shared_ptr<Out> value() = 0;
  bool process()
  {
    return process_impl();
  }

protected:
  virtual bool process_impl() = 0;
};

template < class Out, typename Iterator >
class iterator_element_processor : public stream_element_processor<Out>
{
public:
  typedef Iterator iterator_type;

  iterator_element_processor(iterator_type begin, iterator_type end)
  : value_(begin), end_(end)
  {}

  std::shared_ptr<Out> value() override
  {
    return std::make_shared<Out>(*value_);
  }

protected:
  bool process_impl() override
  {
    if (first_) {
      first_ = false;
      return value_ != end_;
    }
    ++value_;
    return value_ != end_;
  }

private:
  bool first_ = true;
  iterator_type value_;
  iterator_type end_;
};

template < class Out, typename Predicate >
class filter_element_processor : public stream_element_processor<Out>
{
public:
  explicit filter_element_processor(std::shared_ptr<stream_element_processor<Out>> successor, Predicate pred)
  : successor_(std::move(successor))
  , pred_(pred)
  {}

  std::shared_ptr<Out> value() override
  {
    return value_;
  }

protected:
  bool process_impl() override
  {
    if (successor_->process()) {
      value_ = successor_->value();
      if (pred_(*value_)) {
        return true;
      }
      value_.reset();
    }
    return false;
  }

private:
  std::shared_ptr<stream_element_processor<Out>> successor_;
  Predicate pred_;
  std::shared_ptr<Out> value_;
};

template < class In, class Out, typename Predicate >
class map_element_processor : public stream_element_processor<Out>
{
public:
  explicit map_element_processor(std::shared_ptr<stream_element_processor<In>> successor, Predicate pred)
  : successor_(std::move(successor))
  , pred_(pred)
  {}

  std::shared_ptr<Out> value() override
  {
    return value_;
  }

protected:
  bool process_impl() override
  {
    if (successor_->process()) {
      std::shared_ptr<In> val = successor_->value();
      value_ = std::make_shared<Out>(pred_(*val));
      return true;
    }
    value_.reset();
    return false;
  }

private:
  std::shared_ptr<stream_element_processor<In>> successor_;
  Predicate pred_;
  std::shared_ptr<Out> value_;
};


template < class Out, typename Iterator >
std::shared_ptr<stream_element_processor<Out>> make_range(Iterator begin, Iterator end)
{
  return std::make_shared<iterator_element_processor<Out, Iterator>>(begin, end);
}

template < class Out, typename Predicate, typename R = Out>
std::shared_ptr<stream_element_processor<R>> make_filter(Predicate pred, std::shared_ptr<stream_element_processor<Out>> successor)
{
  return std::make_shared<filter_element_processor<Out, Predicate>>(successor, pred);
}

template < class In, typename Predicate, typename Out = typename std::result_of<Predicate&(In)>::type>
std::shared_ptr<stream_element_processor<Out>> make_mapper(Predicate pred, std::shared_ptr<stream_element_processor<In>> successor)
{
  return std::make_shared<map_element_processor<In, Out, Predicate>>(successor, pred);
}

template < class T >
class stream
{
public:
  stream<T>& skip(size_t val)
  {
    return *this;
  }

  template < typename Predicate >
  stream<T>& filter(Predicate pred)
  {
    return *this;
  }

  template < typename Predicate, typename R = typename std::result_of<Predicate&(T)>::type>
  stream<R> map(Predicate pred)
  {
    return stream<R>();
  }

  std::vector<T> to_vector()
  {
    return std::vector<T>();
  }

  std::list<T> to_list()
  {
    return std::list<T>();
  }

private:
  std::shared_ptr<stream_element_processor<T>> proccessor_;
};

template < class T, template < class ... > class C >
stream<T> make_stream(C<T> &&container);

template < class T, template < class ... > class C >
stream<T> make_stream(const C<T> &container);

template < class T >
stream<T> make_stream(std::list<T> &&container)
{
  return stream<T>();
}

template < class T >
stream<T> make_stream(const std::list<T> &container)
{
  return stream<T>();
}

bool is_even(int i) { return i % 2 == 0; }
std::string to_string(int i) { return typeid(i).name() + std::to_string(i); }

int main()
{
  std::list<int> ints { 1, 2, 3, 4, };

  auto range = make_range<int>(std::begin(ints), std::end(ints));

  auto filter = make_filter([](const int &in) {
    return in % 2 == 0;
  }, range);

//  *filter->value() = 8;
//  filter->process();
//  std::cout << "Filter: " << *filter->value() << "\n";

  auto mapper = make_mapper([](const int &in) {
    return typeid(in).name() + std::to_string(in);
  }, filter);

  mapper->process();
  //  std::cout << "Mapper: " << mapper.process(19) << "\n";

  auto s = make_stream(ints);

  s.filter(is_even)
  .map(to_string)
  .to_vector();

  return 0;
}
