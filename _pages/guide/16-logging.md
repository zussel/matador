## Logging

Matador comes with a simple logging system. The main concept uses internally instances of class ```log_domain```. This class is the connection point between a list of log sinks defining where the log messages are written to and one or more instances of class ```logger```. The ```logger``` let the user write a message with a certain log level for a defined source name.

All these components are managed with the ```log_manager``` singleton class.

### Log sinks

A log sink is a defined destination for log messages. Sinks are added to a certain ```log_domain```. They are added as ```std::shared_ptr<log_sink>``` so that they can be shared over
several domains.

By now there are four sink types:

 - ```file_sink```: Log message is written to one file
 - ```stdout_sink```: Log message is written to stdout
 - ```stderr_sink```: Log message is written to stderr
 - ```rotating_file_sink```: Log message is written to several rotating files

#### File sink

A file sink writes the log messages to a single file. The file is defined by a given path. If the complete path doesn't exists, all necessary directories are created.

The helper function ```matador::create_file_sink(...)``` creates the
file sink within a ```std::shared_ptr```.

{% highlight cpp linenos %}
auto path = matador::os::build_path("my", "log", "log.txt");

auto logsink = matador::create_file_sink(path);
{% endhighlight %}

#### Stdout sink

The stdout sink writes the raw message to the standard output.

The helper function ```matador::create_stdout_sink()``` creates the
file sink within a ```std::shared_ptr```.

{% highlight cpp linenos %}
auto stdoutsink = matador::create_stdout_sink();
{% endhighlight %}

#### Stderr sink

The stdout sink writes the raw message to the standard error.

The helper function ```matador::create_stderr_sink()``` creates the
file sink within a ```std::shared_ptr```.

{% highlight cpp linenos %}
auto stdoutsink = matador::create_stderr_sink();
{% endhighlight %}

#### Rotating file sink

The rotating file sink writes the log message into a set of rotating
files. The user can define the maximum number of log files and the
maximum file size for one log file. The file is defined by a given path.

In the following example the rotating log files are located at ```./my/log/```. Where active log file name is ```log.txt```. The max file size is 30 bytes and a maximum of 3 log files are allowed. Once the max file size is reached. The next file is created. This might be one of ```log-{1-3}.txt```.

{% highlight cpp linenos %}
auto path = matador::os::build_path("my", "log", "log.txt");

auto logsink = matador::create_rotating_file_sink(path, 30, 3);
{% endhighlight %}

### Log domain

The ```log_domain``` must have a unique name and bundles a bunch of log sinks. There is always a default ```log_domain``` where sinks can be added to.

Log sinks must be added to a specific domain. To add a sink to a domain call ```matador::add_log_sink(...)```. Called only with the sink to add, the sink is added to the default domain.

{% highlight cpp linenos %}
auto logsink = matador::create_file_sink("log.txt");

// the sink will be added to the default domain
matador::add_log_sink(logsink);
{% endhighlight %}

If called with the sink and a domain name, the sink is added to that
```log_domain```. If the domain doesn't exists, it is created.

{% highlight cpp linenos %}
auto logsink = matador::create_file_sink("log.txt");

// add sink to the domain "net"
matador::add_log_sink(logsink, "net");
{% endhighlight %}

### Log Manager

The ```log_manager``` class acts like a singleton and stores all available ```log_domain``` instances. It is used to add ```log_domain``` instances and to create ```logger``` instances. The ```log_manager``` acts in the background. It is not reallay necessary to call it directly.

All interfaces like adding a ```log_domain``` or to creating a ```logger``` are wrapped through global functions in the matador namesapce.

### Logger

Once all domains and sinks are configured, ```logger``` can be spread over the code to log messages of all log levels.

Each instance of logger consists of a name representing the source of the log message. The source name is also written to the log sink.

The class ```logger``` provides also an interface to write log messages with an apropriate log level. There are six valid levels:

- Fatal messages (```log_level::LVL_FATAL```)
- Error messages (```log_level::LVL_ERROR```)
- Warning messages (```log_level::LVL_WARN```)
- Info messages (```log_level::LVL_INFO```)
- Debug messages (```log_level::LVL_DEBUG```)
- Trace messages (```log_level::LVL_TRACE```)

Lets asume there is a class ```NetworkManager``` with a method ```send_data(...)```. When this method is called a debug log message should be written. Therefor one could place a logger as a member
of the class.

{% highlight cpp linenos %}
class NetworkManager
{
public:
    // init the logger
    NetworkManager()
        : log_(matador::create_logger("NetworkManager"))
    {}

    void send_data(char *data, size_t size)
    {
        // do stuff and the log message
        log_.debug("written %d bytes of data", size);
    }

private:
    matador::logger log_;
}
{% endhighlight %}

In line ```[6]``` the logger is created. The created logger uses the default domain with its configured sinks. The source name of the logger is _"NetworkManager"_.

In method ```send_data(...)``` the logger is called to write a debug message. The interface is the same as the ```::printf(...)``` interface. There are placeholders of variables in the message followed by a list of values.

The resulting log message line looks like this:

```2020-05-04 11:09:15.841 [DEBUG  ] [NetworkManager]: written 678 bytes of data```

The line consists of a timestamp with milliseconds, the log level, the source name and the message.

**Note:** By now you can't change the order or the format of the message.
{: .bs-callout .bs-callout-warning}

The following lists the log interface of the ```logger``` class:

 - ```logger::fatal(message, ...)```
 - ```logger::error(message, ...)```
 - ```logger::warn(message, ...)```
 - ```logger::info(message, ...)```
 - ```logger::debug(message, ...)```
 - ```logger::trace(message, ...)```
