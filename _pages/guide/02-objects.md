## Objects

All kind of object up from POD can be attached to the object store. It can be a
simple struct with publiv members or a complex class hierarchy with bases classes and virtual
methods.

The only thing that must exist is a `serialize` function.

{% highlight cpp linenos %}
struct person
{
    identifier<long> id;
    std::string name;
    unsigned age;

    template < class SERIALIZER >
    void serialize(SERIALIZER &serializer)
    {
        serializer.serialize("id", id);
        serializer.serialize("name", name);
        serializer.serialize("age", age);
    }
};
{% endhighlight %}

Provide such a method for all objects to be attached to the object store and you're done.
