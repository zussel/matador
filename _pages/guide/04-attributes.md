## Attributes

The following types are supported and can be used within the ```matador::access::attribute()``` functions:

### Numbers

All arithmetic number types are supported (```char```, ```short```, ```int```, ```long```,
```long long```, ```unsigned char```, ```unsigned short```, ```unsigned int```, ```unsigned long```,
```unsigned long long```, ```float```, ```double``` and ```bool```).

The ```attribute()``` function takes the operator, the name of the attribute, attribute itself and an optional
```field_attribute``` parameter.

```cpp
struct numbers
{
  char char_ = 'c';
  short short_ = -127;
  int int_ = -65000;
  long long_ = -128000;
  long long long64_ = -32128000;
  unsigned char unsigned_char_ = 'u';
  unsigned short unsigned_short_ = 128;
  unsigned int unsigned_int_ = 65000;
  unsigned long unsigned_long_ = 128000;
  unsigned long long unsigned_long64_ = 32128000;
  bool bool_ = true;
  float float_ = 3.1415f;
  double double_ = 1.1414;

  template < typename Operator >
  void process(Operator &op)
  {
    namespace field = matador::access;
    field::attribute(op, "val_char", char_);
    field::attribute(op, "val_short", short_);
    field::attribute(op, "val_int", int_);
    field::attribute(op, "val_long", long_);
    field::attribute(op, "val_long64", long64_);
    field::attribute(op, "val_unsigned_chr", unsigned_char_);
    field::attribute(op, "val_unsigned_short", unsigned_short_);
    field::attribute(op, "val_unsigned_int", unsigned_int_);
    field::attribute(op, "val_unsigned_long", unsigned_long_);
    field::attribute(op, "val_unsigned_long64", unsigned_long64_);
    field::attribute(op, "val_bool", bool_);
    field::attribute(op, "val_float", float_);
    field::attribute(op, "val_double", double_);
  }
};
```

### Strings

There are three types of supported strings (```std::string``` and ```const char*```). A varchar type can be represented by a string combined with a length in its ```attribute()``` function. See example below

```cpp
struct strings
{
  enum { CSTR_LEN=255 };

  char cstr_[CSTR_LEN];
  std::string string_ = "world";
  std::string varchar_ = "earth";

  template < typename Operator >
  void process(Operator &op)
  {
    matador::access::attribute(op, "val_cstr", cstr_, (std::size_t)CSTR_LEN);
    matador::access::attribute(op, "val_string", string_);
    matador::access::attribute(op, "val_varchar", varchar_, 255); // varchar of length 255
  }
};
```

### Time and date

matador comes with its own [time](../api/classmatador_1_1time) and [date](../api/classmatador_1_1date) classes.
The ```attribute()``` interface is also straight forward.

{% highlight cpp linenos %}
struct time_and_date
{
  matador::date date_;
  matador::time time_;

  template < typename Operator >
  void process(Operator &op)
  {
    matador::access::attribute(op, "val_date", date_);
    matador::access::attribute(op, "val_time", time_);
  }
};
{% endhighlight %}

### Primary keys

Currently two types of  primary keys are supported: Integral type and
varchar e.g. string with size. The process method for integral types looks like this:

{% highlight cpp linenos %}
struct identifier_type_integral
{
  unsigned long id{};

  template < typename Operator >
  void process(Operator &op)
  {
    matador::access::primary_key(op, "id", id);
  }
};
{% endhighlight %}

Below an example of a string primary key. Note that the size parameter is mandatory.
{% highlight cpp linenos %}
struct identifier_type_string
{
  std::string id{};

  template < typename Operator >
  void process(Operator &op)
  {
    matador::access::primary_key(op, "id", id, 255);
  }
};
{% endhighlight %}

### Foreign Objects

Foreign objects are supported by using ```object_ptr<Type>```. The kind of relation is identified
in the ```process()``` method with functions ```has_one()``` and ```belongs_to()```.

{% highlight cpp linenos %}
using namespace matador;

struct foreign_key
{
  object_ptr<T> has_one_;         // has one relationship
  object_ptr<T> belongs_to_;      // belongs to relationship

  template < typename Operator >
  void process(Operator &op)
  {
    access::has_one(op, "has_one_table", has_one_, cascade_type::ALL);
    access::belongs_to(op, "belongs_to_table", belongs_to_, cascade_type::ALL);
  }
};
{% endhighlight %}

```object_ptr<Type>``` is just a link to a foreign object. The second parameter
is the name of the foreign table. With the fourth parameter in the process method
one adjust the cascading command behavior when an insert, update, delete and
select operation is executed on the containing object.
```matador::cascade_type::ALL``` means that all operations
are also passed to the foreign object (e.g. when containing object is
deleted the foreign is deleted as well).


### Collections

Collections are supported by ```container<Type, ContainerType>```. By now ```container``` supports
internal two container types ```std::vector``` (default) and ```std::list```.

{% highlight cpp linenos %}
struct relations
{
  matador::container<T> collection_; // a collection with objects

  template < typename Operator >
  void process(Operator &op)
  {
    matador::access::has_many(op, "collection", collection_, "local_id", "foreign_id");
  }
};
{% endhighlight %}

```container<Type>``` describes a collection leading to a relation table/object between
the containing object and the foreign object. If the foreign object is of
a simple builtin type (e.g. ```int``` or ```matador::date```) the value
itself is stored in the relation table.
