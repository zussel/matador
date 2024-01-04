## Persistence

The ORM layer makes internally use of the fluent query interface and the object store. The
query interface is used to define the queries for all attached entities and the object store
acts like an internal cache for the concrete entity objects.

**Note:** By now the ORM layer loads the database in whole. Which means internally all the
data is loaded from the database into the object store. I plan to implement a lazy loading
mechanism later.
{: .bs-callout .bs-callout-warning}

Before you can use the ORM layer you have to setup the persistence layer. This means to
configure all entities you want to use. For that you need an instance of class
```persistence``` and a database connection string.
Once such an object is created you can attach entities in the same way it is done with
the ```object_store```.

{% highlight cpp linenos %}
    persistence p("sqlite://db.sqlite");
    p.attach<person>("person");
    p.attach<student, person>("student");
    p.attach<course>("course");
{% endhighlight %}

Now you can create the database schema simply by calling ```create```:

{% highlight cpp linenos %}
    p.create();
{% endhighlight %}
Or you can drop it as well:

{% highlight cpp linenos %}
    p.drop();
{% endhighlight %}
After that is done you can deal with a session object and start building your app.
