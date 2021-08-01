## Introduction

The following documentation will give you an overview on matador. It will show you all main components of the library so that you can start building your own application with it.

matador consists of four main parts: A container for any kind of objects, a sql query class providing a fluent interface, an ORM layer and top a simple web server.

### The Object Store

The [object store](#prototypes) is the central element. Once it is configured with an object hierarchy, one can insert, update or delete any of objects of the introduced types. Create a view to access all objects of a specific type or of a specific base type. Once the [view](#views) is created all objects can be iterated or filtered via a simple [expression](#expressions). As a special feature a [transaction](#transactions) mechanism is also integrated.

{% highlight cpp linenos %}
struct person
{
  matador::identifier<long> id;
  std::string name;

  person(const std::string &n) : name(n) {}

  template < typename S >
  void serialize(S &s) {
    s.serialize("id", id);
    s.serialize("name", name);
  }
};

int main()
{
  matador::object_store store;
  store.attach<person>("person");

  store.insert(new person("georg"));

  matador::object_view<person> view(store);
}
{% endhighlight %}

### The SQL Query

With the [query](#querries) class at hand one can write simple sql statements, prepare and execute.

{% highlight cpp linenos %}
matador::query<person> q("person");

// select all person with name 'jane' or 'tarzan' from database
auto col = column("name");
auto res = q.select().where(col == "jane" || col == "tarzan").execute();

for (auto p : res) {
  std::cout << "my name is " << p->name << "\n";
}
{% endhighlight %}


### The ORM Layer

On top of the object store and the query interface comes the persistence layer. It represents the ORM mechanism provided by the library. Once a [persistence](#persistence) object is created one can again configure the object hierarchy in the same way it is done with the object store.

{% highlight cpp linenos %}
matador::persistence p("sqlite://db.sqlite");
p.attach<person>("person");
{% endhighlight %}

After the persistance layer is configured all the database tables can be created

{% highlight cpp linenos %}
p.create();
{% endhighlight %}

Now you can create a session and insert, update, select or delete entities on the
database.

{% highlight cpp linenos %}
session s(p);

auto jane = s.insert(new person("jane"));

// set janes real age
jane.modify()->age = 35;
s.flush();

// bye bye jane
s.remove(jane)
s.flush();
{% endhighlight %}
Once you have data in your database you can load it this way:

{% highlight cpp linenos %}
session s(p);

s.load();
{% endhighlight %}

### The HTTP Server

With the web server it is possible to write a web application and serve the data as REST Api or as full HTML pages. To make live a bit easier a template engine was added based on the [Django Template language](https://docs.djangoproject.com/en/3.2/ref/templates/language/).