## Json

Matador provides a simple and lightweight json module including a parser, an object mapper for pod types and an object mapper for matador type entities.

### Creating Json objects

To use Json object you have to include ```#include "matador/utils/json.hpp```. Json supports the following datatypes:

- Booleans (represented by ```bool```)
- Numbers (represented by ```double``` or ```long long```)
- Strings (represented by ```std::string```)
- Objects (represented by ```std::map<std::string, json>```)
- Arrays (represented by ```std::vector<T>```)
- Null (represented by ```std::nullptr_t```)

#### Create json with standard types

Creating a Json object without initialization is initialized with null value.

{% highlight cpp linenos %}
json js;

// is true
js.is_null();
{% endhighlight %}

Its also possible to initialize a json object with a certain value.

{% highlight cpp linenos %}
json ji(7);
json jb(true);
json jd(3.5);
json js("hallo");
json jnill(nullptr);
{% endhighlight %}

#### Create json object

When it comes to objects or arrays it is as easy as handling standard types:

{% highlight cpp linenos %}
// create an empty object
json jo = json::object();

// is true
jo.is_object();

// assigning values
jo["name"] = "george";
jo["age"] = 35;
jo["weight"] = 78.5;
jo["male"] = true;
{% endhighlight %}

#### Create json array

Handling of json arrays is as easy as handling json objects:

{% highlight cpp linenos %}
// create an empty array
json ja = json::array();

// is true
jo.is_array();

ja.push_back("green");
ja.push_back(15);
ja.push_back(true);
{% endhighlight %}

### Type checking
A json object can be checked on a spcific type

{% highlight cpp linenos %}
ji.is_number(); // returns true on arithmentic types (e.g. double, long, etc.)
ji.is_integer(); // returns true on integer types
jb.is_boolean(); // checks on boolean type
jd.is_real(); // checks on real type
js.is_string(); // checks on string type
jo.is_object(); // checks on object type
ja.is_array(); // checks on array type
jnill.is_null();
{% endhighlight %}

### Accessing json values

The value of the json can be accessed via the ```as<T>()``` method. The requested type is passed with template
parameter. If the json type doesn't match the requested type and exception ist thrown.

{% highlight cpp linenos %}
auto i = js.as<int>();
auto s = js.as<std::string>();
{% endhighlight %}

In case the json is an object one can use the ```[]``` operator with the key string of the requested json value.
If the key leads to a valid json this is returned otherwise a json null value is created and returned for the given key.

{% highlight cpp linenos %}
auto jo = json::object();
jo["name"] = "george";

// string "george" is returned
auto s = jo["name"];

// a json of type null is returned
auto n = jo["age"];
{% endhighlight %}

In case the json is an array one can also use the ```[]``` operator but with the index of the requested json value. If the index leads to a valid json this is returned otherwise am exception is thrown.

{% highlight cpp linenos %}
auto jo = json::array();
jo.push_back("george");

// string "george" is returned
auto s = jo[0];

// throws an exception
auto n = jo[1];
{% endhighlight %}

### Object Mapping

Matador comes with a builtin simple json object mapping mechanism. To be able to use it, the object to create must have the ```serialize()``` interface implemented.

{% highlight cpp linenos %}
struct person
{
    identifier<long> id;
    std::string name;
    matador birthday;

    template < class SERIALIZER >
    void serialize(SERIALIZER &serializer)
    {
        serializer.serialize("id", id);
        serializer.serialize("name", name);
        serializer.serialize("birthday", birthday);
    }
};
{% endhighlight %}

Now an object mapper can be created and an object or array can be serialized

{% highlight cpp linenos %}
json_mapper<person> mapper;

auto p = mapper.object_from_string(R"({
    "id": 8,
    "name": "george",
    "birthday": "1987-09-27"
})")

std::cout << p.name << "\n";
{% endhighlight %}

The ```json_mapper``` provides also an interface to serialize an array of objects. The returned
value is a ```std::vector``` of the serialized objects.

{% highlight cpp lineos %}
auto vec = mapper.array_from_string(R"(  [...]  )");

std::cout << vec.size() << "\n";
{% endhighlight %}

The ```json_mapper``` supports all default types and the matador types ```identifier```, ```date``` and ```time```. If an error occurs while parsing the string an ```json_exception``` is thrown.

### Matador Object Mapping

Matador also provides a json object mapper supporting the matador relation types ```has_one```, ```belongs_to``` and ```has_many```. Therefor a special mapper ```json_object_mapper``` is available.

Asume there is a simple class ```address```

{% highlight cpp lineos %}
struct address
{
    matador::identifier<unsigned long> id;
    std::string street;
    std::string city;
    matador::belongs_to<citizen> citizen_;

    address() = default;
    address(const std::string &str, const std::string &c)
        : street(str), city(c)
    {}

    template < class SERIALIZER >
    void serialize(SERIALIZER &serializer)
    {
        serializer.serialize("id", id);
        serializer.serialize("street", street, 255);
        serializer.serialize("city", city, 255);
        serializer.serialize("citizen", citizen_, matador::cascade_type::NONE);
    }
};
{% endhighlight %}

This address is used in a ```citizen``` class which is derived from the ```person``` class:

{% highlight cpp lineos %}
struct citizen : public person
{
    citizen() = default;
    explicit citizen(const std::string &name, const matador::date &bdate, unsigned h
        : person(name, bdate, h) {}

    matador::has_one<address> address_;

    template < class SERIALIZER >
    void serialize(SERIALIZER &serializer)
    {
        serializer.serialize(*matador::base_class<person>(this));
        serializer.serialize("address", address_, matador::cascade_type::ALL);
    }
};
{% endhighlight %}

There we have a relationship between both classes. Now the ```json_object_mapper``` can serialize a
json string of this relationship:

{% highlight cpp lineos %}
json_object_mapper<citizen> mapper;

auto p = mapper.object_from_string(R"(  {
    "id":  5, "name": "george", "height": 185,
    "birthdate": "2001-11-27", "address": {
        "id": 4, "street": "east-street",
        "city": "east-city", "citizen": 5
    } 
} )");

std::cout << p.address_->city << "\n";
{% endhighlight %}

The ```json_object_mapper``` provides also an interface to serialize an array of objects. The returned
value is a ```std::vector``` of the serialized objects.

{% highlight cpp lineos %}
auto vec = mapper.array_from_string(R"(  [...]  )");

std::cout << vec.size() << "\n";
{% endhighlight %}

If an error occurs a ```json_exception``` is thrown.
