---
title: "Objects"
---
All kind of object up from POD can be attached to the object store. It can be a
simple struct with publiv members or a complex class hierarchy with bases classes and virtual
methods.

The only thing that must exist is a `serialize` function.

```cpp
struct person
{
    std::string name;
    unsigned age;

    template < class SERIALIZER >
    void serialize(SERIALIZER &serializer)
    {
        serializer.serialize("name", name);
        serializer.serialize("age", age);
    }
};
```

Provide such a method for all objects to be attached to the object store and you're done.

## Supported types

The following types are supported and can be used within the ```serialize()``` method:

### Numbers

All arithmetic number types are supported (```char```, ```short```, ```int```, ```long```,
```unsigned char```, ```unsigned short```, ```unsigned int```, ```unsigned long```,
```float```, ```double``` and ```bool```).

The ```serialize()``` method takes the name of the attribute and attribute itself.

```cpp
struct numbers
{
  char char_ = 'c';
  short short_ = -127;
  int int_ = -65000;
  long long_ = -128000;
  unsigned char unsigned_char_ = 'u';
  unsigned short unsigned_short_ = 128;
  unsigned int unsigned_int_ = 65000;
  unsigned long unsigned_long_ = 128000;
  bool bool_ = true;
  float float_ = 3.1415f;
  double double_ = 1.1414;

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize("val_char", char_);
    serializer.serialize("val_short", short_);
    serializer.serialize("val_int", int_);
    serializer.serialize("val_long", long_);
    serializer.serialize("val_unsigned_chr", unsigned_char_);
    serializer.serialize("val_unsigned_short", unsigned_short_);
    serializer.serialize("val_unsigned_int", unsigned_int_);
    serializer.serialize("val_unsigned_long", unsigned_long_);
    serializer.serialize("val_bool", bool_);
    serializer.serialize("val_float", float_);
    serializer.serialize("val_double", double_);
  }

};
```

### Strings

There are three types of supported strings (```std::string```, ```const char*``` and
```oos::varchar<S>```). The latter is a string type representing database type VARCHAR
taking its size as the template argument.

```cpp
struct strings
{
  enum { CSTR_LEN=255 };

  char cstr_[CSTR_LEN];
  std::string string_ = "world";
  oos::varchar<255> varchar_ = "earth";

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize("val_cstr", cstr_, (std::size_t)CSTR_LEN);
    serializer.serialize("val_string", string_);
    serializer.serialize("val_varchar", varchar_);
  }
};
```

### Time ad date

OOS comes with its own [time](/docs/time) and [date](/docs/date) classes.
The ```serialize()``` interface is also straight forward.

```cpp
struct time_and_date
{
  oos::date date_;
  oos::time time_;

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize("val_date", date_);
    serializer.serialize("val_time", time_);
  }
};
```

### Primary keys

There is also a special type ```identifier<T>``` providing primary key
behavior. The serializer looks like this:

```cpp
struct identifier_type
{
  oos::identifier<unsigned long> id_ = 0;

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize("id", id_);
  }
};
```

### Foreign Objects

Foreign objects are supported by simply using ```has_one<T>```. This class acts as a wrapper
around the object of type ```T```. It can be accessed like a pointer.

```cpp
struct foreign_key
{
  oos::has_one<T> foreign_;         // just a foreign object

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize("foreign", foreign_, oos::cascade_type::ALL);
  }
};
```

```has_one<T>``` is just a link to a foreign object. With the third parameter
in the serialize method one adjust the cascading command behavior when
an insert, update, delete and select operation is executed on the
containing object. ```oos::cascade_type::ALL``` means that all operations
are also passed to the foreign object (e.g. when containing object is
deleted the foreign is deleted as well).


### Collections

Collections are supported by ```has_many<T, Container>```. By now ```has_many``` supports
two container types ```std::vector``` (default) and ```std::list```.

```cpp
struct relations
{
  oos::has_many<T> collection_; // a collection with objects

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize("collection", collection_, "local_id", "foreign_id");
  }
};
```

```has_many<T>``` describes a collection leading to a relation table/object between
the containing object and the foreign object. If the foreign object is of
a simple builtin type (e.g. ```int``` or ```oos::varchar<S>```) the value
itself is stored in the relation table.
