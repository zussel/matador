#ifndef MATADOR_ROUTE_PATH_HPP
#define MATADOR_ROUTE_PATH_HPP

#include "matador/http/http.hpp"

#include <functional>
#include <regex>

namespace matador {
namespace http {

class request;
class response;

typedef std::unordered_map<std::string, std::string> t_path_param_map;
typedef std::function<response(const request&, const t_path_param_map &path_params)> t_request_handler;

/*
 * path
 * path_regex
 * method
 */
class route_endpoint
{
public:
  route_endpoint(std::string path_spec, http::method_t method, t_request_handler request_handler);

  bool match(const std::string &path, t_path_param_map &path_params);

private:
  std::string path_spec_;
  std::regex path_regex_;
  http::method_t method_;
  t_request_handler request_handler_;
};

class route_path
{
public:
  typedef std::unordered_map<std::string, std::string> t_path_param_map;
  typedef std::function<response(const request&, const t_path_param_map &path_params)> t_request_handler;


  route_path() = default;
  route_path(std::string endpoint_name, std::string endpoint_path, http::method_t method);
  route_path(std::string endpoint_name, std::string endpoint_path, http::method_t method, t_request_handler request_handler);

  virtual ~route_path() = default;

  virtual bool match(const std::string &path, t_path_param_map &path_params);

  std::string endpoint_name() const;

  const std::string& endpoint_path() const;
  void endpoint_path(const std::string &path);

  http::method_t method() const;
  void method(http::method_t method);

  template < class RequestHandler >
  void request_handler(RequestHandler rh)
  {
    request_handler_ = rh;
  }

  response execute(const request &req, const t_path_param_map &path_params);

  friend bool operator==(const route_path &a, const route_path &b);
  friend bool operator!=(const route_path &a, const route_path &b);
  friend bool operator<(const route_path &a, const route_path &b);
  friend bool operator<=(const route_path &a, const route_path &b);
  friend bool operator>(const route_path &a, const route_path &b);
  friend bool operator>=(const route_path &a, const route_path &b);

private:
  std::string endpoint_name_;
  std::string endpoint_path_;
  http::method_t method_ = http::UNKNOWN;
  t_request_handler request_handler_;
};

class path_param_route_path : public route_path
{
public:
  path_param_route_path(
    std::string param_name,
    std::string endpoint_name,
    std::string endpoint_path,
    http::method_t method,
    t_request_handler request_handler
  );

  bool match(const std::string &path, t_path_param_map &path_params) override;

protected:
  std::string param_name_;
};

class regex_path_param_route_path : public path_param_route_path
{
public:
  regex_path_param_route_path(
    const std::string& reg,
    std::string param_name,
    std::string endpoint_name,
    std::string endpoint_path,
    http::method_t method,
    t_request_handler request_handler
  );

  bool match(const std::string &path, t_path_param_map &path_params) override;

private:
  std::regex param_regex_;
};

class static_file_path : public route_path
{
public:
  static_file_path(
    std::string file_pattern,
    std::string endpoint_name,
    std::string endpoint_path,
    http::method_t method,
    t_request_handler request_handler
  );

  bool match(const std::string &path, t_path_param_map &path_params) override;

private:
  std::string file_pattern_;
};

}
}
#endif //MATADOR_ROUTE_PATH_HPP
