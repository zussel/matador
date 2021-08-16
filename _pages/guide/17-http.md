## Http

Based on the matador [network stack](#networking) a simple http server and client are also provided. These classes can handle
the HTTP methods __GET__, __POST__, __PUT__ and __DELETE__. The data is exchanged with a ```http::request``` and the result comes
within a ```http::response```.

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

* ```on_post(const std::string &route, RequestHandler request_handler)```
* ```on_put(const std::string &route, RequestHandler request_handler)```
* ```on_remove(const std::string &route, RequestHandler request_handler)```

These four can be used to build up your REST server. The callback hides behind the ```RequestHandler``` and takes a ```http::request```
as a parameter. The return value of this function is a ```http::response``` object.

### Http Client

### Middleware

### Template Engine

More coming soon with the next release (0.8.0)
