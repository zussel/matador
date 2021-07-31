## Networking

Matador comes with a simple network stack realized through the reactor pattern. The groundwork is realized through the ```address```, ```peer``` and ```socket``` classes. Based on these classes the reactor consists of the ```reactor```, ```àcceptor```, ```connector``` and the an implementation of a ```handler``` handling an open network connection.

### Protocol

The reactor supports the __TCP__ and the __UDP__ protocoll. Based on these to protocolls there two classes ```tcp``` and ```udp```. These classes provides shortcuts to the socket and peer types.

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

### Reactor

The ```reactor``` class implements a __single threaded__ ```select()``` based reactor pattern.

**Note:** In a future release the reactor will be multi threaded to increase performance.
{: .bs-callout .bs-callout-warning}

### Acceptor

### Connector

### Handler

### IO Service

### IO Stream

### Stream Handler

Finally the ```stream_handler``` class implements the ```handler``` and 
the ```io_stream``` interface.

More coming soon with the next release (0.8.0)
