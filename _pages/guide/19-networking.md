## Networking

Matador comes with a simple network stack realized through the reactor pattern. The groundwork is realized through the ```address```, ```peer``` and ```socket``` classes. Based on these classes the reactor consists of the ```reactor```, ```àcceptor```, ```connector``` and the an implementation of a ```handler``` handling an open network connection.

### Protocol

The reactor supports the __TCP__ and the __UDP__ protocoll. Based on these to protocolls there are two classes ```tcp``` and ```udp```. These classes provides shortcuts to the socket and peer types.

Therefor two classes ```tcp``` and ```udp``` with type definitions for ```peer```, ```socket```, ```acceptor``` and (address) ```resolver``` exists in header ```ip.hpp```.

### Network Address

The ```address``` class represents either an IPv4 or an IPv6 address. An IpV4 or IPv6 address can easily created with the following lines:

{% highlight cpp linenos %}
auto google_v4 = address::v4::from_hostname("www.google.org");
auto google_v6 = address::v6::from_hostname("www.google.org");
{% endhighlight %}

In the same way an address can be created from an IP address:

{% highlight cpp linenos %}
auto localhost_v4 = address::v4::from_ip("127.0.0.1");
auto localhost_v6 = address::v6::from_ip("::1");
{% endhighlight %}

There are also shortcuts for __broadcast__, __loopback__ and __any__ addresses:

{% highlight cpp linenos %}
address broadcast_v4 = address::v4::broadcast();
address broadcast_v6 = address::v6::broadcast();

address localhost_v4 = address::v4::loopback();
address localhost_v6 = address::v6::loopback();

address any_v4 = address::v4::any();
address any_v6 = address::v6::any();
{% endhighlight %}

### Peer

A peer constists of an address and a port. It represents a network connection endpoint of either the local or the remote side.

Below see an example to create a peer for localhost at port 8080:

{% highlight cpp linenos %}
address localhost = address::v4::loopback();
tcp::peer localhost8080(localhost, 8080);
{% endhighlight %}

### Socket

A socket represents a network connection between two network endpoints (peers). The connection can either be connected or closed.

### Handler

The ```handler``` class provides an interface for all connection processors handled by the ```reactor```. The concrete implementation must
overwrite interface

The following interfaces might be implemented depending on the purpose of the concrete handler implementation:

* ```on_input()```: Called every time the connection has data to read
* ```on_output()```: Called every time the connection has data to write
* ```on_except()```: Called every time the connection has exceptionell data available
* ```on_timeout()```: Called every time a scheduled timeout for this handlers occurred
* ```on_close()```: Called when the connection is about to close
* ```open()```: Called when an handler is opened
* ```close()```: Called when an handler is closed
* ```is_ready_write()```: Called by the reactor to check if there is data ready to write
* ```is_ready_read()```: Called by the reactor to check if there is data ready to read
* ```name()```: Returns a name for this handler

### Acceptor

The ```acceptor``` class implements the ```handler``` interface and accepts a connection within the running
reactor. It needs an endpoint to listen on and an accept handler which is called once a connection was established.
It returns an implementation of the ```handler``` class which is then registered within the reactor.

Assuming there is a class ```EchoHandler``` handling an established connection the creation
of an acceptor could look like the code below:

{% highlight cpp linenos %}
// create an endpoint (listens on port 7777)
auto ep = tcp::peer(address::v4::any(), 7777);

// creates the acceptor with endpoint and handler callback
auto ac = std::make_shared<acceptor>(ep, [](tcp::socket sock, tcp::peer p, acceptor *) {
    auto cl = std::make_shared<EchoHandler>();
    cl->init(std::move(sock), std::move(p));
    return cl;
  });
{% endhighlight %}

### Connector

The ```connector``` class implements the ```handler``` interface and connects to a given connection endpoint and
once the connection is established by the remote side a connect handler callback is called. It returns an implementation
of the ```handler``` class which is then registered within the reactor.

{% highlight cpp linenos %}
// create the handler in advance
auto echo_conn = std::make_shared<EchoHandler>();

// creates the connector with handler callback
auto echo_connector = std::make_shared<connector>([echo_conn](const tcp::socket& sock, const tcp::peer &p, connector *) {
    echo_conn->init(sock, p);
    return echo_conn;
  });
{% endhighlight %}

### Reactor

The ```reactor``` class implements a __single threaded__ ```select()``` based reactor pattern. Interally it deals with a list 
of handlers for reading, writing and accepting io data or to schedule a call in a given interval.

**Note:** In a future release the reactor will be multi threaded to increase performance.
{: .bs-callout .bs-callout-warning}

Once a ```reactor``` is instanciated handlers can be registered with ```register_handler(handler, event_type)``` where
handler comes within a shared_ptr and the event type tells the reactor the use case of the handler (_reading_, _writing_,
_accepting_, _exceptionell_).

Most common usage is here to register an accetpor handler processing incoming connections.

After the handler are registered the reactor can be started.

{% highlight cpp linenos %}
auto ac = std::make_shared<accpector>(...);

reactor r;

r.register_handler(ac, event_type::ACCEPT_MASK);

r.run();
{% endhighlight %}

The ```reactor``` provides also the possibility to schedule a timer calling a handler on timeout. Therefor
the ```schedule_timer(handler, offset, interval)``` method can be used. The scheduling starts once the
reactor is started.

{% highlight cpp linenos %}
auto h = std::make_shared<TimeoutHandler>(...);

reactor r;
// calls on_timeout() after 1 second and than every 3 seconds
r.schedule_timer(h, 1, 3);

r.run();
{% endhighlight %}

### IO Service

The ```io_service``` encapsulates the ```reactor``` and provides an easy to use interface
to register callbacks for established connections either active ones (```connect()```) or
passive ones (```accept()```).

### IO Stream

The ```io_stream``` defines an interface used by the ```io_service```. There are two main
interface methods ```read(buffer_view, read_handler)``` and ```write(buffer_view_list, write_handler)```.

The first interface is called when data should be read from a socket. Once the date was read
the given _read\_handler_ is called.

The second nterface is called when data should be written to a socket. Once the data was
written the given write handler is called.

### Stream Handler

Finally the ```stream_handler``` class implements the ```handler``` and 
the ```io_stream``` interface and is used by the ```io_stream``` class.

It handles the reading and writing of data for every established connection.
