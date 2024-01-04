## Datatypes

Matador comes with some own datatypes and helper classes. The datatype classes represents ```time```, ```date``` and ```identifier<T>``` (which stands for a primary key).
The helper class ```varchar<SIZE>``` eases the creation of ```has_many``` relations or identifiers with varchars.

### Time

matador comes with its own simple time class. It represents a time with milliseconds
precisions. Once a time object exists it can be modified, copied or assigned. For full
documentation see the [api](../api/classmatador_1_1time/).

#### Creation

Time can be created from several constructors.

| Constructor | purpose |
|-------------|---------|
|```time()``` | Creates a time of now |
|```time(timt_t)```| Creates a time of ```time_t``` value|
|```time(timeval)```| Creates a time of struct ```timeval```|
|```time(int, int, int, int, int, int, long)```| Creates time from year, month, day, hour, minutes, seconds and optional milliseconds. The given parameters are validated.|

The obvious copy and assignment constructors exists as well as a static parsing function

{% highlight cpp linenos %}
matador::time t = matador::time::parse("03.04.2015 12:55:12.123", "%d.%m.%Y %H:%M:%S.%f");
{% endhighlight %}

The parse format tokens are the same as the ones from [```strptime```](https://linux.die.net/man/3/strptime)
plus the ```%f``` for the milliseconds.

#### Display

The time consists of an stream output operator which displays the time in ISO8601 format

{% highlight cpp linenos %}
matador::time t(2015, 1, 31, 11, 35, 7, 123);

std::cout << t;
{% endhighlight %}

Results in:

```bash
2015-01-31T11:35:07
```

There is also a ```to_string()``` function taking the time as first parameter and a format
string as second parameter. It returns the formatted time as ```std::string```.

{% highlight cpp linenos %}
matador::time t(2015, 1, 31, 11, 35, 7, 123);

std::cout << to_string(t, "%H:%M:%S.%f %d.%m.%Y");
{% endhighlight %}

Results in:

```bash
11:35:07.123 31.01.2015
```

#### Modify

To modify a time one can use the fluent interface allowing the user to concatenate
all parts to be modified in sequence.

{% highlight cpp linenos %}
matador::time t(2015, 1, 31, 11, 35, 7);
// modification
t.year(2014).month(8).day(8);
{% endhighlight %}

#### Conversions

The time can be converted into a [```matador::date```](../api/classmatador_1_1date/)


{% highlight cpp linenos %}
matador::time t(2015, 1, 31, 11, 35, 7);

matador::date d = t.to_date();
{% endhighlight %}

There are also methods to retrieve ```tm``` and ```timeval``` struct:

{% highlight cpp linenos %}
matador::time t(2015, 1, 31, 11, 35, 7);

struct tm ttm = t.get_tm();
struct timeval tv = t.get_timeval();
{% endhighlight %}

### Date

matador comes with its own simple date class. It represents a date consisting of year, month
and day. The underlying calendar is a julian date calendar. Once a date object exists it
can be modified, copied or assigned. For full documentation see the [api](../api/classmatador_1_1date/).

#### Creation

Date can be created from several constructors.

| Constructor | purpose |
|-------------|---------|
|```date()``` | Creates a date of now |
|```date(int)``` | Creates a date from julian date|
|```date(const char *d, const char *format)``` | Creates a date from date stamp with the given format|
|```date(int, int, int)``` | Creates a date from day, month and year|

The obvious copy and assignment constructors exists as well.

#### Display

The date consists of an stream output operator which displays the date in ISO8601 format

{% highlight cpp linenos %}
matador::date d(31, 1, 2015);

std::cout << d;
{% endhighlight %}

Results in:

```bash
2015-01-31
```

There is also a ```to_string()``` function taking the date as first parameter and a format
string as second parameter. It returns the formatted date as ```std::string```.

{% highlight cpp linenos %}
matador::date d(31, 1, 2015);

std::cout << to_string(d, "%d.%m.%Y");
{% endhighlight %}

Results in:

```bash
31.01.2015
```

#### Modify

To modify a date one can use the fluent interface allowing the user to concatenate
all parts to be modified in sequence.

{% highlight cpp linenos %}
matador::date d(31, 1, 2015);
// modification
d.year(2014).month(8).day(8);
{% endhighlight %}

The operators ```++```, ```--```, ```+=``` and ```-=``` are also available and increase or decrease
the date by one day or the given amount of days for the latter two operators.

{% highlight cpp linenos %}
matador::date d(31, 1, 2015);
// modification

d += 4;

std:cout << d;
{% endhighlight %}

Leads to

```bash
2015-02-04
```

#### Conversions

The date can be retrieved as julian date value:

{% highlight cpp linenos %}
matador::date d(31, 1, 2015);

std::cout << "julian date: " << d.julian_date();
{% endhighlight %}
