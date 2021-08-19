## Http

Based on the matador [network stack](#networking) a simple http server and client are also provided. These classes can handle
the HTTP methods __GET__, __POST__, __PUT__ and __DELETE__. The data is exchanged with a ```http::request``` and the result comes
within a ```http::response```.

### Request

When dealing with HTTP communication there is always a ```request``` object representing a
specific HTTP request. In __matador__ there also a ```http::request```.

Im most cases the request is either created by the ```http::client``` class or by the request
parser called with the ```http::server```. When accessing and evaluating the request in a
regsitered callback the user can access all neccessary information from the request object.

The accessors of the request class are self explanatory:

{% highlight cpp linenos %}
class request
{
  http::method_t method() const;
  std::string url() const;
  std::string fragment() const;
  http::version version() const;
  std::string host() const;
  const http::content& content() const;
  const t_string_param_map& headers() const;
  const t_string_param_map& path_params() const;
  const t_string_param_map& query_params() const;
  const t_string_param_map& form_data() const;
  const std::string& body() const;
};
{% endhighlight %}

### Response

The ```http::response
### Http Server

With the ```http::server``` class it is easy to create a simple HTTP server in a few lines of code.
Just instanciate a ```http::server```, enable the builtin routing middleware and add a callback for a route.
The start the server.

{% highlight cpp linenos %}
http::server server(8000);
server.add_routing_middleware();

server.on_get("/", [](const http::request &) {
    return http::response::ok("hello world", http::mime_types::TYPE_PLAIN_TEXT);
});

server.run();
{% endhighlight %}

Besides ```on_get(...)``` there are three other callback taking methods:

* ```on_get(const std::string &route, RequestHandler request_handler)```
* ```on_post(const std::string &route, RequestHandler request_handler)```
* ```on_put(const std::string &route, RequestHandler request_handler)```
* ```on_remove(const std::string &route, RequestHandler request_handler)```

These four methods can be used to build up your REST server. The callback hides behind the ```RequestHandler``` and takes a ```http::request```
as a parameter. The return value of this function is a ```http::response``` object.

After build up the HTTP callbacks the server can be started with ```http::server::run()```.

### Http Client

The ```http::client``` enables the user to write a http client and call remote HTTP functions
like __GET__, __POST__, __PUT__ and __DELETE__. The client is by now synchronous waiting for an answer
of the remote side.

{% highlight cpp linenos %}
http::client client("www.remote.net:8000");

http::response resp =  client.get("/hello");
{% endhighlight %}

Besides ```get(...)``` there are three other methods used to send a request:

* ```get(const std::string &route)```
* ```post(const std::string &route, const std::string &body)```
* ```put(const std::string &route, const std::string &body)```
* ```remove(const std::string &route)```

### Middleware

The ```http::server``` uses the middleware pattern (which is a variation of the [Chain of Responsibility](https://refactoring.guru/design-patterns/chain-of-responsibility/cpp/example)
pattern.

### Template Engine

More coming soon with the next release (0.8.0)
