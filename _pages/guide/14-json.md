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

### Retrieving json values

To retrieve a value from a json object the ```as<T>()``` can be used. It
accesses the underlying concrete value und tries to convert it to the
given type.
There is a type checking with given template type defining that the
type fits to the json type. Use the following table to find the
corresponding convertion. If the type checking fails a ```logic_error```
is thrown.

|Template type   |Json type     |
|----------------|--------------|
|integral        |integer number|
|floating point  |real number   |
|bool            |boolean       |
|string,char*    |string        |

### Mapping of objects

There are to classes providing json mapping functionality ```json_mapper``` and
```json_object_mapper```. Where the first maps builtin types and std containers
and the second one can handle matador types like ```has_one``` and ```has_many```.

#### Json mapper

The ```json_mapper``` class comes with three kind of interfaces. One to convert
to a json formatted string. Another to convert to a json object and the third one
to convert to a object or a list of objects.

The following methods take a json, an object or a list of objects and as a second
parameter a json format object and returns a formatted json string.

- ```to_string(json, format)```
- ```to_string<T>(object, format)```
- ```to_string<T>(vector<object>, format)```

The next set of methods takes a json string, an object or a list of objects and
convert it into a json object.

- ```to_json(string)```
- ```to_json<T>(object)```
- ```to_json<T>(vector<object>)```

The last set of methods takes a json object or a json string and converts it into
an object of the requested type of into a list of objects.

- ```to_object<T>(json)```
- ```to_object<T>(string)```
- ```to_objects<T>(json)```
- ```to_objects<T>(string)```

#### Json object mapper

The ```json_object_mapper``` class comes with the same three kind of interfaces as
the ```json_mapper``` but with partially different arguments.

The ```to_string``` methods converts an array of objects, an object pointer or an
object view in combination with a json format definition into a json formatted
string.

- ```to_string<T>(array<objects>, format)```
- ```to_string<T>(object_ptr, format)```
- ```to_string<T>(object_view, format)```

To convert an object pointer or an object view to a json object use the ```to_json(...)```
methods.

- ```to_json<T>(object_ptr)```
- ```to_json<T>(object_view)```

The last set of methods converts a json object or a string into one object or a list
of objects of a specific type.

- ```to_object<T>(json)```
- ```to_object<T>(string)```
- ```to_objects<T>(json)```
- ```to_objects<T>(string)```
