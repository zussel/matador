---
title: Date
---
{% include base_path %}
{% include toc icon="columns" title="Relations" %}

OOS comes with its own simple date class. It represents a date consisting of year, month
and day. The underlying calendar is a julian date calendar. Once a date object exists it
can be modified, copied or assigned. For full documentation see the [api](/api/classoos_1_1date/).

### Creation

Date can be created from several constructors.

| Constructor | purpose |
|-------------|---------|
|```date()``` | Creates a date of now |
|```date(int)``` | Creates a date from julian date|
|```date(const char *d, const char *format)``` | Creates a date from date stamp with the given format|
|```date(int, int, int)``` | Creates a date from day, month and year|

The obvious copy and assignment constructors exists as well.

### Display

The date consists of an stream output operator which displays the date in ISO8601 format

```cpp
oos::date d(31, 1, 2015);

std::cout << d;
```

Results in:

```bash
2015-01-31
```

There is also a ```to_string()``` function taking the date as first parameter and a format
string as second parameter. It returns the formatted date as ```std::string```.

```cpp
oos::date d(31, 1, 2015);

std::cout << to_string(d, "%d.%m.%Y");
```

Results in:

```bash
31.01.2015
```

### Modify

To modify a date one can use the fluent interface allowing the user to concatenate
all parts to be modified in sequence.

```cpp
oos::date d(31, 1, 2015);
// modification
d.year(2014).month(8).day(8);
```

The operators ```++```, ```--```, ```+=``` and ```-=``` are also available and increase or decrease
the date by one day or the given amount of days for the latter two operators.

```cpp
oos::date d(31, 1, 2015);
// modification

d += 4;

std:cout << d;
```

Leads to

```bash
2015-02-04
```

### Conversions

The date can be retrieved as julian date value:

```cpp
oos::date d(31, 1, 2015);

std::cout << "julian date: " << d.julian_date();
```
