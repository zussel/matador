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
