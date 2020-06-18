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

### Retrieving json values

