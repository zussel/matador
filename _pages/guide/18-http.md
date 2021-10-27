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

The ```http::response``` is the counterpart class to ```http::request```. It provides the rqeuested answer
and contains therefor all neccessary fields like headers, body and status code.

There are severval shortcuts to create a response object. A simple OK response can be created in the following way:

{% highlight cpp linenos %}
auto resp = http::response::ok("hello world", http::mime_types::TYPE_PLAIN_TEXT);
{% endhighlight %}

With this call all neccessary fields like body, header, date, length and type are set and the response can be send.

It is also possible to pass objects or json objects into this function but with out the mimetype.

{% highlight cpp linenos %}
template < class T >
static response ok(const T& obj) { //... }
{% endhighlight %}

This will also lead to an OK response with the given object converted into a json strig as body.

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

A middleware must implement the ```process(...)``` interface. The example below shows how
an example logging middleware could be implemented.

{% highlight cpp linenos %}
class log_middleware : public http::middleware
{
public:
  log_middleware()
    : log_(matador::create_logger("LogMiddleware"))
  {}

  matador::http::response process(http::request &, const next_func_t &next) override
  {
    log_.info("before LogMiddleware");
    auto resp = next();
    log_.info("after LogMiddleware");
    return resp;
  }

private:
  matador::logger log_;
};
{% endhighlight %}

When entering the ```process(...)``` method the request itself is passed and a callback
to the next middleware returning a ```http::response``` object. With this setup it is
possible to make an action **before** and **after** the request was processed by all other middlewares.
After the middle has done its **before** action it must call the passed function callback ```next()```
executing the next middleware and returning a response object.

In the **after** region it is possible to access, modify or ignore the response object
returned by the call to the ```next()``` callback.

Once a middleware is implemented, an instance can be added to the HTTP server:

{% highlight cpp linenos %}
http::server svr(...);

srv.add_middleware(std::make_shared<log_middleware>());

// setup server routes
// ...
srv.run();
{% endhighlight %}

### Template Engine

Matador comes also with a small template engine. It uses the [Django Template language](https://docs.djangoproject.com/en/3.2/ref/templates/language/)
syntax but by now not all tags and filters are implemented at all. The advantage is that the langauge isn't
limited on XML/HTML content it can be used with any text based formats.

Just call ```render``` with the content string to be processed and a json object containing the data:

{% highlight cpp linenos %}
std::string content { ... };
json data { ... };

auto result = http::template_engine::render(content, data);
{% endhighlight %}

The returned result string contains the processed content based on the json data.

The engine handles variable and tags. Variables are indicated by two curly brackets __\{\{ \[var\] \}\}__ and
the containing variable is replaced by its value. Tags are like logic commands and are indicated by
one curly bracket and a percent sign __\{\% \[tag\] \%\}__.

#### Tags
The following tags are available:

- ```for``` A for loop
- ```if``` Conditions
- ```include``` Include and parse other files

#### For Loop

The tag __for__ implements the loop functionallity. Loop scan be nested. In the exmaple
below names and items of all persons are listed.

{% highlight django linenos %}
{% raw %}
<ul>
  {% for person in persons %}
  <li>
    <h2>{{ person.name }}</h2>
    <ol>
      {% for i in person.items %}
      <li>{{ i }}</li>
      {% endfor %}
    </ol>
  </li>
  {% endfor %}
</ul>
{% endraw %}
{% endhighlight %}

#### If Condition

The __if__ condition tag works as unary operator (the variable evaluates to true or false) or as
binary operator (the expression evaluates to true or false).

The common expression operators are available (__==__, __!=__, __<__, __<=__, __>__ and __>=__).

{% highlight django linenos %}
{% raw %}
{% if person.id == 2 %}
  <p>1 Details of {{ person.name }}</p>
{% elif person.id < 2 %}
  <p>2 Details of {{ person.name }}</p>
{% elif person.id >= 3 %}
  <p>3 Details of {{ person.name }}</p>
{% else %}
  <p>No details</p>
{% endif %}
{% endraw %}
{% endhighlight %}

#### Include File

#### Filter

And the following filters are available:

- ```escape``` Escapes a string variable
- ```capfirst``` Makes the first character of a string uppercase
- ```upper``` Makes the string uppercase
- ```lower``` Makes the string lowercase

#### Escape filter
#### Capfirst filter
#### Upper filter
#### Lower filter
