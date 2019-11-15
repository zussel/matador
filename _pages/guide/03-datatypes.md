## Datatypes

The following types are supported and can be used within the ```serialize()``` method:

### Numbers

All arithmetic number types are supported (```char```, ```short```, ```int```, ```long```,
```unsigned char```, ```unsigned short```, ```unsigned int```, ```unsigned long```,
```float```, ```double``` and ```bool```).

The ```serialize()``` method takes the name of the attribute and attribute itself.

{% highlight cpp linenos %}
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
{% endhighlight %}

### Strings

There are three types of supported strings (```std::string``` and ```const char*```). A varchar type can be represented by a string combined with a length in its ```serialize``` method. See example below

{% highlight cpp linenos %}
struct strings
{
  enum { CSTR_LEN=255 };

  char cstr_[CSTR_LEN];
  std::string string_ = "world";
  std::string varchar_ = "earth";

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize("val_cstr", cstr_, (std::size_t)CSTR_LEN);
    serializer.serialize("val_string", string_);
    serializer.serialize("val_varchar", varchar_, 255); // varchar of length 255
  }
};
{% endhighlight %}

### Time and date

matador comes with its own [time](../api/classmatador_1_1time) and [date](../api/classmatador_1_1date) classes.
The ```serialize()``` interface is also straight forward.

{% highlight cpp linenos %}
struct time_and_date
{
  matador::date date_;
  matador::time time_;

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize("val_date", date_);
    serializer.serialize("val_time", time_);
  }
};
{% endhighlight %}

### Primary keys

There is also a special type ```identifier<T>``` providing primary key
behavior. The serializer looks like this:

{% highlight cpp linenos %}
struct identifier_type
{
  matador::identifier<unsigned long> id_ = 0;

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize("id", id_);
  }
};
{% endhighlight %}

### Foreign Objects

Foreign objects are supported by simply using ```has_one<T>```. This class acts as a wrapper
around the object of type ```T```. It can be accessed like a pointer.

{% highlight cpp linenos %}
struct foreign_key
{
  matador::has_one<T> foreign_;         // just a foreign object

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize("foreign", foreign_, matador::cascade_type::ALL);
  }
};
{% endhighlight %}

```has_one<T>``` is just a link to a foreign object. With the third parameter
in the serialize method one adjust the cascading command behavior when
an insert, update, delete and select operation is executed on the
containing object. ```matador::cascade_type::ALL``` means that all operations
are also passed to the foreign object (e.g. when containing object is
deleted the foreign is deleted as well).


### Collections

Collections are supported by ```has_many<T, Container>```. By now ```has_many``` supports
two container types ```std::vector``` (default) and ```std::list```.

{% highlight cpp linenos %}
struct relations
{
  matador::has_many<T> collection_; // a collection with objects

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize("collection", collection_, "local_id", "foreign_id");
  }
};
{% endhighlight %}

```has_many<T>``` describes a collection leading to a relation table/object between
the containing object and the foreign object. If the foreign object is of
a simple builtin type (e.g. ```int``` or ```matador::varchar<S>```) the value
itself is stored in the relation table.
