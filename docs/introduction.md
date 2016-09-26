---
title: Introduction
---

{% include base_path %}
{% include toc icon="columns" title="Overview" %}

The following documentation will give you an overview on Open Object Store. It will show you all main components of the library so that you can start building your own application with it.

OOS consists of three main parts: A container for any kind of objects, a sql query class providing a fluent interface and on top an ORM layer.

### The object store

The [object store](/#) is the central element. Once it is configured with an object hierarchy, one can insert, update or delete any of objects of the introduced types. Create a view to access all objects of a specific type or of a specific base type. Once the [view](/#) is created all objects can be iterated or filtered via a simple [expression](/#). As a special feature a [transaction](/#) mechanism is also integrated.

```cpp
struct person
{
  oos::identifier<long> id;
  std::string name;
  
  person(const std::string &n) : name(n) {}
  
  template < typename S >
  void serialize(S &serializer) {
    s.serialize("id", id);
    s.serialize("name", name);
  }
};

int main()
{
  oos::object_store store;
  store.attach<person>("person");
  
  store.insert(new person("georg"));
  
  oos::object_view<person> view(store);
}
```

### The SQL Query

With the [query](/#) class at hand one can write simple sql statements, prepare and execute.

```cpp
oos::query<person> q("person");

// select all person with name 'jane' or 'tarzan' from database
auto col = column("name");
auto res = q.select().where(col == "jane" || col == "tarzan").execute();

auto first = res.begin();
auto last = res.end();
while (first != last) {
  std::cout << "my name is " << (*first++).name << "\n";
}
```

### The ORM Layer

On top of the object store and the query interface comes the persistence layer. It represents the ORM mechanism provided by the library. Once a [persistence](/#) object is created one can again configure the object hierarchy in the same way it is done with the object store.

```cpp
oos::persistence p("sqlite://db.sqlite");
p.attach<person>("person");
```

After the persistance layer is configured all the database tables can be created

```cpp
p.create();
```

If the schema already exists one can call

```cpp
p.load();
```

Now you can create a session and insert, update, select or delete entities on the
database.

```cpp
session s(p);

auto jane = s.insert(new person("jane"));

// set janes real age
jane->age = 35;
s.update(jane);

// bye bye jane
s.remove(jane)
```