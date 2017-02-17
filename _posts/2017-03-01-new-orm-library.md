---
title: "New C++ orm library"
categories:
  - News
tags:
  - C++
  - ORM
  - OOS
excerp: 'Announcement of my new C++ ORM libry OOS'
---
{% include base_path %}
{% include toc icon="columns" title="Modules" %}

I'm happy to announce a new version of my C++ ORM library **OOS** 0.5.0. This library
aims to help you writing database applications without worrying about SQL statements,
transactions and serialization.

OOS provides access to three databases _**MySQL/MaraiDB**_, _**MS SQL Server**_ and
_**SQLite 3**_.

Furthermore OOS can be divided in three modules

_ORM layer_
:   A transaction based ORM layer

_Fluent SQL query interface_
:   Provides a way to write a query in a fluent why just concatenating the
parts of the query

_Object store container_
:   A container holding all objects of any type once introduced to the container.
  Has the ability to create views on object types to iterate and filter them with
  simple expressions.

Despite being the fourth release of this library this version comes with a lot of breaking
changes.
 
### Transaction based ORM layer

On top sits an [ORM layer](/docs/persistence) with a simple API letting you define your objects without
deriving them from a base object class. You can adjust your existing entity classes
with minor changes.

Assume a simple ```person``` class with a couple of members you wrote:

```cpp
struct person
{
  unsigned long id;
  std::string name;
  unsigned int height;
  time_t birthdate;
  person *mother = nullptr;
  person *father = nullptr;
};
```

This class can be adjusted to fit in **OOS** orm layer like that:

```cpp
struct person
{
  oos::identifier<unsigned long> id;
  oos::varchar<255> name; // leads to a varchar column of 255 chars 
  unsigned int height;
  oos::date birthdate; // uses oos::date class  
  oos::has_one<person> mother;
  oos::has_one<person> father;
};
```

The last thing missing is a serialization method. Which is simply defined like this:

```cpp
struct person
{
  // members ...
  
  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize("id", id);
    serializer.serialize("name", name);
    serializer.serialize("height", height);
    serializer.serialize("birthdate", birthdate);
    serializer.serialize("mother", mother);
    serializer.serialize("father", father);
  }
};
```

Now we can setup our database layout with an instance of ```persistence```. When creating
a persistence object we have to pass the database connection string.

```cpp
oos::persistence p("sqlite://test.db");
p.attach<person>("person");

// create tables
p.create();
```

With ```persistence::attach<T>()``` a new object type can be defined where the given string
is the name of the table to be created. The call to ```create()``` creates the database tables.

Now we can create a ```session``` object. With this object we can modify our database. Once an
object is created it wrapped by an ```object_ptr<T>```. To be safe only access your object through
this wrapper. Otherwise 

```cpp
oos::session s(p); // create session from persistence object

// start a transaction
oos::transaction tr = s.begin();
try {
  auto georg = s.insert(new person("georg")); // leads to an insert statement
  
  george->height = 183; // leads to an update statement
  
  // commit all changes
  tr.commit();
} catch(exception &ex) {
  std::cout << "exception caught: ex.what()" << "\n";
  tr.rollback();
}
```

It is also possible to call ```insert```, ```update``` and ```remove``` without a transaction.

```cpp
s.remove(georg); // leads to a delete statement
```

### Fluent SQL query interface

If there's no need in the ORM functionality but one in a unified SQL query interface one
can use the **OOS** fluent query interface. This interface provides a way to write queries
like writing a sql statement. All you need is a database connection and a
[query object](/docs/queries).

The ```query``` object is a template class where the default template type is of class
 ```row```. That means you have to take care of the columns and values of the table. See
 the example below.
  
```cpp
oos::connection conn("sqlite://test.db");
conn.open();

oos::query<> q("product", conn);

// create product table
q.create({
  make_typed_id_column<long>("id"),
  make_typed_varchar_column<255>("name"),
  make_typed_column<unsigned long>("material"),
  make_typed_column<double>("weight")
}).execute();

// insert a product
q
  .insert({"id", "name", "material", "weight"})
  .values({1, "stone", "concrete", 2.5})
  .execute();
  
// select products
column material("material");
auto res = q
  .select({"name", "weight"})
  .where(material == "stone")
  .execute();
```
If you want to use an available object class (i.e. ```product``` is already
a class) you can use this class as template parameter with the query. Than the
example from above looks much cleaner:

```cpp
oos::connection conn("sqlite://test.db");
conn.open();

oos::query<product> q("product", conn);

// create product table
q.create().execute();

// create a product, assuming there is a constructor
product p1(1, "stone", "concrete", 2.5)

// insert a product
q.insert(p1).execute();
  
// select products
column material("material");
auto res = q
  .select({"name", "weight"})
  .where(material == "stone")
  .execute();
```

### One container for all objects
