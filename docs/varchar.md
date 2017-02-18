---
title: Varchar
---
{% include base_path %}

The ```varchar<T>``` class represents a VARCHAR database data type. Internally it contains
a ```std::string```. The template parameter takes the capacity of the varchar. If the assigned
string is exceeds the capacity, the string is truncated on database. For full documentation
see the api of [varchar_base](/api/classoos_1_1varchar__base/) and [varchar](/api/classoos_1_1varchar/).

### Creation

A ```varchar``` is simply created:

```cpp
// without value
oos::varchar<255> name;

// with initial value
oos::varchar<255> name("hello world");
```

### Display

The ```varchar<T>``` has an overloaded output stream operator. So it can be written to every
stream as usual

```cpp

oos::varchar<255> title("My title");

std::cout << "Title: " << title;
```

Leads to

```bash
Title: My title
```

### Modification

There are a few modification methods and operators like

```cpp
oos::varchar<255> title("My title");

title += " is great";

std::cout << "Title: " << title;
```

Results in

```bash
Title: My title is great
```

Assignment is available for other ```varchar<T>``` objects, std::string and character pointers.

```cpp
oos::varchar<255> title("My title");
oos::varchar<255> other_title("My other title");

title.assign(other_title);

std::cout << "Title: " << title;
```

Results in

```bash
Title: My other title
```
