---
title: Time
---
OOS comes with its own simple time class. It represents a time with milliseconds
precisions. Once a time object exists it can be modified, copied or assigned. For full
documentation see the [api](/api/classoos_1_1time/).

### Creation

Time can be created from several constructors.

| Constructor | purpose |
|-------------|---------|
|```time()``` | Creates a time of now |
|```time(timt_t)```| Creates a time of ```time_t``` value|
|```time(timeval)```| Creates a time of struct ```timeval```|
|```time(int, int, int, int, int, int, long)```| Creates time from year, month, day, hour, minutes, seconds and optional milliseconds. The given parameters are validated.|

The obvious copy and assignment constructors exists as well as a static parsing function

```cpp
oos::time t = oos::time::parse("03.04.2015 12:55:12.123", "%d.%m.%Y %H:%M:%S.%f");
```

The parse format tokens are the same as the ones from [```strptime```](https://linux.die.net/man/3/strptime)
plus the ```%f``` for the milliseconds.

### Display

The time consists of an stream output operator which displays the time in ISO8601 format

```cpp
oos::time t(2015, 1, 31, 11, 35, 7, 123);

std::cout << t;
```

Results in:

```bash
2015-01-31T11:35:07
```

There is also a ```to_string()``` function taking the time as first parameter and a format
string as second parameter. It returns the formatted time as ```std::string```.

```cpp
oos::time t(2015, 1, 31, 11, 35, 7, 123);

std::cout << to_string(t, "%H:%M:%S.%f %d.%m.%Y");
```

Results in:

```bash
11:35:07.123 31.01.2015
```

### Modify

To modify a time one can use the fluent interface allowing the user to concatenate
all parts to be modified in sequence.

```cpp
oos::time t(2015, 1, 31, 11, 35, 7);
// modification
t.year(2014).month(8).day(8);

```

### Conversions

The time can be converted into a [```oos::date``](/docs/date)


```cpp
oos::time t(2015, 1, 31, 11, 35, 7);

oos::date d = t.to_date();
```

There are also methods to retrieve ```tm``` and ```timeval``` struct:

```cpp
oos::time t(2015, 1, 31, 11, 35, 7);

struct tm ttm = t.get_tm();
struct timeval tv = t.get_timeval();
```
