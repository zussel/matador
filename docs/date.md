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

### Modify

### Conversions