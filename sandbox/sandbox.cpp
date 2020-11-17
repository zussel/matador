#include <type_traits>
#include <iostream>


struct Foo
{
  void bar() {
    std::cout << "Foo::bar called\n";
  }

  template < typename FN, typename std::enable_if<std::is_member_function_pointer<FN>::value>::type* = nullptr >
  void doit(FN fn)
  {
    (this->*fn)();
  }
};

int main(int /*argc*/, char* /*argv*/[])
{
  Foo foo;

  foo.doit(&Foo::bar);

  return 0;
}

//  http::server serv;
//
//  serv.on_get("/", [](http::request &request) {
//    return http::response;
//  });
//
//  serv.listen(7090);

// server
  /*
  GET / HTTP/1.1
  Host: localhost:7090
  User-Agent: curl/7.70.0
  Accept: * / *
  */

// client
//(HTTP/1.1 200 OK
//Server: Matador/0.7.0
//Content-Length: 111
//Content-Language: de
//Connection: close
//Content-Type: text/html
//
//<!doctype html>
//<html>
//  <head>
//    <title>Dummy!</title>
//  </head>
//  <body>
//    <p>Help!</p>
//  </body>
//</html>
