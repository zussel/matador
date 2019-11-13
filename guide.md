---
title: Guide
layout: guide
permalink: /guide/
---
<div class="col-xs-12 col-sm-9 guide">

## Introduction

The following documentation will give you an overview on matador. It will show you all main components of the library so that you can start building your own application with it.

matador consists of three main parts: A container for any kind of objects, a sql query class providing a fluent interface and on top an ORM layer.

### The object store

The [object store](#prototypes) is the central element. Once it is configured with an object hierarchy, one can insert, update or delete any of objects of the introduced types. Create a view to access all objects of a specific type or of a specific base type. Once the [view](#views) is created all objects can be iterated or filtered via a simple [expression](#expressions). As a special feature a [transaction](#transactions) mechanism is also integrated.

{% highlight cpp linenos %}
struct person
{
  matador::identifier<long> id;
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
jane->age = 35;
s.update(jane);

// bye bye jane
s.remove(jane)
{% endhighlight %}
Once you have data in your database you can load it this way:

{% highlight cpp linenos %}
session s(p);

s.load();
{% endhighlight %}

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

There are three types of supported strings (```std::string```, ```const char*``` and
```matador::varchar<S>```). The latter is a string type representing database type VARCHAR
taking its size as the template argument.

{% highlight cpp linenos %}
struct strings
{
  enum { CSTR_LEN=255 };

  char cstr_[CSTR_LEN];
  std::string string_ = "world";
  matador::varchar<255> varchar_ = "earth";

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize("val_cstr", cstr_, (std::size_t)CSTR_LEN);
    serializer.serialize("val_string", string_);
    serializer.serialize("val_varchar", varchar_);
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

## Prototypes

Once all entities are designed you want to store them in the object store. Before this could be done
all entities must be introduced into the object store.

Assume we have an abstract base class ```Vehicle``` and derived from this the
classes ```Truck```, ```Car``` and ```Bike```. Now lets make this hierarchy known to the
matador::object_store:

{% highlight cpp linenos %}
matador::object_store ostore;
ostore.attach<Vehicle>("vehicle", true);
ostore.attach<Truck, Vehicle>("truck");
ostore.attach<Car, Vehicle>("car");
ostore.attach<Bike, Vehicle>("bike");
{% endhighlight %}

As you can see it is quite simple to add the hierarchy to
the matador::object_store by calling method ```matador::object_store::attach```.
As there are several ways to call this method we decide to use the one
with template arguments.

In the example above the vehicle class is an abstract base. Here we need only one template
argument (the class itself: Vehicle). With the first method parameter you give your
type a unique name. The second parameter is a flag telling the ```matador::object_store```
that this type is abstract. Settings this flag to true you can't insert objects of this
abstract type.

### Inserting Objects

Now that we've setup up our hierarchy we can insert objects into
the matador::object_store.

{% highlight cpp linenos %}
typedef object_ptr<vehicle> vehicle_ptr;

vehicle_ptr truck = ostore.insert(new Truck("MAN");
vehicle_ptr car   = ostore.insert(new Car("VW Beetle");
vehicle_ptr bike  = ostore.insert(new Bike("Honda");
{% endhighlight %}
As you can see we use matador::object_ptr of type vehicle. The vehicle class in
our example is the abstract base class for all concrete vehicle types.
So the concrete vehicle object is inserted correctly and assigned to the
object_ptr.

That means once you have inserted an object of any concrete type you access
it via an appropriate object_ptr afterwards. You should never work with the
raw instance pointer. This could lead to inconsistencies.

### Updating Objects

Now that we have some objects inserted we may want
to modify them. The important thing here is as mentioned above
that you don't deal with raw pointer to your object when try
to modify it. You always have a pointer object wrapped
around the object (like shared_ptr).
The ```matador::object_store``` returns an ```matador::object_ptr``` when an
object is inserted. Once you received the ```matador::object_ptr```
you can change your object by using it like usual pointer.

{% highlight cpp linenos %}
typedef matador::object_ptr<Truck> truck_ptr;
truck_ptr truck = ostore.insert(new Truck("MAN"));

truck->weight = 2.5;
truck->color = red;
{% endhighlight %}
**Note:** When using the raw pointer object store can't guarantee that a change is
properly rolled back when an exception occurs.
{: .bs-callout .bs-callout-warning}

### Deleting Objects

An already inserted object can be deleted from the object store using the ```remove```
method:

{% highlight cpp linenos %}
typedef matador::object_ptr<Truck> truck_ptr;
truck_ptr truck = ostore.insert(new Truck("MAN"));

if (ostore.is_removable(truck)) {
  // remove object
  ostore.remove(truck);
} else {
  // object can't be removed
}
{% endhighlight %}
The object store will check if the object is removable (i.e. there are no other objects
referencing this object). If the object is not removable an ```object_exception``` is
thrown.
One can do this check manually by calling simply ```object_store::is_removable``` (as shown
in the example above).

## Relations

When it comes to object relations you can use one to one, one to many and
many to many relations in a straight forward way. Therefor object store provides
easy wrapper for these relations

### OneToOne Relations

In this example we have two object types an address class
and a person class. The address class acts as the ```belongs_to``` part
of the relation. So we add a ```matador::belongs_to<person>``` to our address
class. Don't forget to add the appropriate ```serialize``` to the
serialize method.

For the person in address we add the ```cascade_type::NONE```. That means if
the address is removed the person won't be removed.

{% highlight cpp linenos %}
struct address
{
  std::string street;
  std::string city;
  matador::belongs_to<person> citizen;

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    // ...
    serialize("citizen", citizen, cascade_type::NONE);
  }
};
{% endhighlight %}

In the persons declaration we add an ```matador::has_one<address>``` and
the call to ```serialize``` to the class. Here we use ```cascade_type::ALL```.
That means if the person is removed the address is removed as well.
That's it. Now we have a one to one relationship beetween two classes.

{% highlight cpp linenos %}
struct person
{
  // ...
  matador::hans_one<address> addr;

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    // ...
    serialize("address", addr, cascade_type::ALL);
  }
};
{% endhighlight %}

**Node:** With this kind of relationship we have a hard linked
relationship. Which means if we remove the person from
our store the address object is removed as well. The
```cascade_type::ALL``` means the all operation of
```INSERT```, ```UPDATE``` and ```DELETE``` will take
affect on the member address as well
{: .bs-callout .bs-callout-warning}

When using this construct the matador will take care of the following:

- If a person object is set into address' ```citizen``` field the persons
```address``` field is automatically updated with this address.
- If an address object is set into persons ```address``` field the address'
```citizen``` field is automatically updated with this person.

{% highlight cpp linenos %}
// setup session/object_store
auto george = s.insert(new person("george"));
auto home = s.insert(new address("homestreet", "homecity"));

george->addr = home;

// person george will be set into address
std::cout << "citizen: " << home->citizen->name << "\n";
{% endhighlight %}

### OneToMany Relations

When it comes to one to many relationships it is also
quiet easy. matador comes with a handy wrapper to the STL container
class ```std::vector``` and ```std::list```.

**Node:** The STL container class ```std::set``` and ```std::unordered_set```
will be supported soon.
{: .bs-callout .bs-callout-warning}

Because these classes are designed in the same way as
the STL classes we can use them in the same way plus the
benefit of the relationship.

We change our handy ```person``` class that it has a lot of addresses.
The ```address``` class can stay untouched because the ```belongs_to``` part
doesn't need to change.

{% highlight cpp linenos %}
struct person
{
  std::string name;
  matador::has_many<address> addresses;

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    // ...
    serialize("address", addresses, "person_id", "address_id");
  }
};
{% endhighlight %}
Now we can add several addresses to a person object and address' ```citizen```
field is filled again automatically.
But it works also in the opposite way: If a person is set into an address the
address is automatically added to persons address list.

{% highlight cpp linenos %}
// ...
// create a new person
auto joe = s.insert(new person("joe"));
auto home = s.insert(new address("homestreet", "homecity"));
auto work = s.insert(new address("workstreet", "workcity"));

joe->addresses.push_back(home);
// homes citicen will be joe
std::cout << "citizen: " << home->citizen->name << "\n";

work->citizen = joe;
// joes addresses have now increased to two
std::cout << "joes addresses: " << joe->addresses.size() << "\n";
{% endhighlight %}

Now we can simply iterate over the list like we used to
do it with all STL containers..

{% highlight cpp linenos %}
// access all friends
for (auto addr : joe->addresses) {
  std::cout << "address street: " << addr->street << "\n";
}
{% endhighlight %}

### ManyToMany Relations

Many to many relationships can also be used straight forward. Asume we
have a class ```student``` taking a list of courses and a class ```course```
having a list of students.

{% highlight cpp linenos %}
struct student
{
  matador::has_many<course> courses;

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    // ...
    serialize("student_course", courses, "student_id", "course_id");
  }
};

struct course
{
  matador::has_many<student> students;

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    // ...
    serialize("student_course", students, "student_id", "course_id");
  }
};
{% endhighlight %}
Once a student adds a course to its course list the student is added to the list
of students of the course. And the other way around if a student is added to a course
the course is added to the list of students course list.

{% highlight cpp linenos %}
auto jane = s.insert(new student("jane"));
auto art = s.insert(new course("art"));
auto algebra = s.insert(new course("algebra"));

jane->courses.push_back(art);
std::cout << art->students.front()->name << "\n"; // prints out 'jane'

art->students.push_back(jane);
std::cout << jane->courses.size() << "\n"; // prints out '2'
{% endhighlight %}

## Views

In most cases we want to iterate over all objects of a certain
type. How can we achieve this? We open a view for the type we want
to inspect.

Again we use our little person class and insert some persons into
our store.

{% highlight cpp linenos %}
// add some friends
ostore.insert(new person("joe"))
ostore.insert(new person("walter"));
ostore.insert(new person("helen"));
ostore.insert(new person("tim"));
{% endhighlight %}

Than we create a view with the ```matador::object_view``` class. This class
takes as the template parameter the desired class type. Then we can
use the view like a STL list containing ```matador::object_ptr``` of our
desired type.

{% highlight cpp linenos %}
// shortcut to the person view
typedef matador::object_view<person> person_view_t;

person_view_t pview(ostore);

person_view_t::iterator i = pview.begin();
for (i; i != pview.end(); ++i) {
  std::cout << "person: " << i->name << std::endl;
}
{% endhighlight %}

But this class can to somethig more for us. If we have a hierarchy
of classes, we can create a view of the base type and easily iterate
overall sub-types.

Look at the next example. We have a hierarchy with the person class
as base class and inherited from that we have classes student and
employee.
And again we create a view of type person to access all objects of
type person including sub-types student and employee.

{% highlight cpp linenos %}
class student : public person { //... };
class employee : public person { //... };

matador::object_store ostore;
ostore.attach<person>("person");
ostore.attach<student, person>("student");
ostore.attach<employee, person>("employee");

typedef matador::object_view<person> person_view_t;

person_view_t pview(ostore);

person_view_t::iterator i = pview.begin();
for (i; i != pview.end(); ++i) {
  std::cout << i->name() << std::endl;
}
{% endhighlight %}

## Expressions

When working with an ```matador::object_view``` you may want to find
a certain object or you want to apply a function only for
some of the objects in the view.

To achieve this we can use the ```matador::object_view::find_if``` method
or we can use the global ```::for_each_if()``` function. But how can we
define or filter criterion?

First add some elements (persons) to the store and create the view.
As you can see we've extended the person class with an age attribute.

{% highlight cpp linenos %}
// add some persons
ostore.insert(new person("joe", 45))
ostore.insert(new person("walter", 56));
ostore.insert(new person("helen", 37));
ostore.insert(new person("tim", 14));
ostore.insert(new person("linda", 25));
ostore.insert(new person("lea", 30));
ostore.insert(new person("georg", 42));

typedef matador::object_view<person> person_view_t;
typedef person_view_t::iterator iterator;

person_view_t pview(ostore);
{% endhighlight %}

If we want to find the person of age 25 we can achieve this by using
the ```matador::object_view::find_if``` method. But first we have to create a
variable holding the method of our object which should be used for
comparation. Here it is ```person::age```.

{% highlight cpp linenos %}
// create the variable
matador::variable<int> x(matador::make_var(&person::age));

// find the person of age 25
iterator i = pview.find_if(x == 25);
{% endhighlight %}

The call returns the first occurence of the object matching
the expression or the end iterator.

If you want to apply a function to all objects matching a
certain expression use ```matador::for_each_if()``` and use it in the
following described way.

{% highlight cpp linenos %}
// print all persons with age between 20 and 60 years
void print(const person_ptr &p)
{
  std::cout << p->name() << " is between 40 and 60.\n";
}

// declare a variable/literal for the age
matador::variable<int> x(matador::make_var(&person::age));

// use the for_each_if function
for_each_if(pview.begin(), pview.end(), x > 20 && x < 60, print);
{% endhighlight %}

All persons whos age lays between 40 and 60 are printed.

## Transactions

A handy feature ist the transaction mechanism provided by the ```object_store```. Once an
object is inserted, updated or deleted the change is not reversible. If you use a transaction
it is. Therefor a transaction must be started:

{% highlight cpp linenos %}
matador::object_store store;
store.attach<person>("person");

matador::transaction tr(store);

try {
  tr.begin();

  auto hans = store.insert(new person("Hans"));

  /// do some stuff and commit
  tr.commit();
} catch (std::exception &) {
  tr.rollback();
}
{% endhighlight %}

Once a transaction is started all changed are tracked until a ```commit``` is called. When it
comes to rollback a transaction ```roolback``` must be called (i.e an exception is thrown).

It is also possible to nest transaction:

{% highlight cpp linenos %}
matador::transaction outer(store);

outer.begin();
// do something and start a second transaction

matador::transaction inner(store);
inner.begin();
// change stuff and commit inner transaction
inner.commit();
// commit outer
outer.commit();
{% endhighlight %}

## Databases

To connect to a database you have to create and open a connection. Therefor you need a
connection object. This is initialized with a connection string. The connection string commes
in an uri way:

```
<database-type>://[<username>[:<password>]@][<server>]/<database-name>
```
- __```database-type:```__ One of ```mysql```, ```mssql``` or ```sqlite```
- __```username:```__ The username of the database connection
- __```password:```__ The password of the database connection
- __```server:```__ The server address or hostname of the database
- __```database-name:```__ The name of the database instance


__Example:__

{% highlight cpp linenos %}
connection conn("mysql://test@localhost/mydb");

conn.open();
// ... use connection
conn.close();
{% endhighlight %}

### Supported Databases

There're currently three supported databases and the in memory database.
But more databases will follow. Next is the description of the
database connection string for the supported databases.

#### MS SQL Server

{% highlight cpp linenos %}
// MSSQL connection string with password
session ses(ostore, "mssql://user:passwd@host/db");

// MSSQL connection string without password
session ses(ostore, "mssql://user@host/db");
{% endhighlight %}

#### MySQL

{% highlight cpp linenos %}
// MySQL connection string with password
session ses(ostore, "mysql://user:passwd@host/db");

// MySQL connection string without password
session ses(ostore, "mysql://user@host/db");
{% endhighlight %}

#### SQLite

{% highlight cpp linenos %}
// MySQL connection string
session ses(ostore, "sqlite://database.sqlite");
{% endhighlight %}

## Queries

On the low level side of the library resides the fluent query interface. It allows you
to write a SQL query in safe way without concerning about the current SQL dialect. There
are two type of queries a typed one and an anonymous one dealing with a ```row``` object.

#### Introdcution

Once you have established a connection to yout database you can execute a query.

{% highlight cpp linenos %}
matador::connection conn("sqlite://test.sqlite");

conn.open();
// create a typed query
matador::query<person> q("person");

// create the table based on the given type
q.create().execute(conn);
{% endhighlight %}
You can use an anonymous query as well:

{% highlight cpp linenos %}

matador::query<> q;
auto res = count.select({columns::count_all()}).from("person").execute(*conn);

int count = res.begin()->at<int>(0);
{% endhighlight %}

**Note:** The query interface represents not the full command syntax. By now it provides
basic functionality to ```create```, ```drop```, ```insert```, ```update```, ```select```
and ```delete``` a table.
{: .bs-callout .bs-callout-warning}

#### Create

The ```create``` method is used to create a database table. The typed version looks
like this:

{% highlight cpp linenos %}
matador::query<person> q("person");

// create the table based on the given type
q.create().execute(conn);
{% endhighlight %}

When using the anonymous version you have to describe the fields of the table to create:

{% highlight cpp linenos %}
matador::query<> q("person");
// Todo: implement functionality
q.create({
    make_typed_id_column<long>("id"),
    make_typed_varchar_column<std::string>("name"),
    make_typed_column<unsigned>("age")
}).execute(conn);
{% endhighlight %}

#### Drop

The ```drop``` method is used to drop a table. The typed usage is as follows:

{% highlight cpp linenos %}
matador::query<person> q("person");

// create the table based on the given type
q.drop().execute(conn);
{% endhighlight %}
The anonymous version is like this:

{% highlight cpp linenos %}
matador::query<> q;

// create the table based on the given type
q.drop("person").execute(conn);
{% endhighlight %}

#### Insert

Inserting an object is done as simple. Just pass the object to the ```insert``` method
and you're done.

{% highlight cpp linenos %}
matador::query<person> q("person");

person jane("jane", 35);

q.insert(jane).execute(conn);
{% endhighlight %}

When building an anonymous insert statement one has to column and value fields like that

{% highlight cpp linenos %}
matador::query<> q("person");

q.insert({"id", "name", "age"}).values({1, "jane", 35}).execute(conn);
{% endhighlight %}

#### Update

Updating an object works the same way as inserting an object. Asume there is an object
one can modify it and pass it to the ```update``` method. Notice the where clause with
expression to limit the update statement. These conditions are explained [condition chapter](#conditions) bewlow

{% highlight cpp linenos %}
person jane("jane", 35);
matador::query<person> q("person");
// insert ...
jane.age = 47;
matador::column name("name");

q.update(jane).where(name == "jane").execute(conn);
{% endhighlight %}

If you're dealing with an anonymous row query the update query looks like the example below. As
you can see, it is simple done with initializer list and pairs of columns and their new
values.

{% highlight cpp linenos %}
matador::query<> q("person");

matador::column name("name");
q.update({
    {"name", "otto"},
    {"age", 47}
}).where(name == "jane");
{% endhighlight %}

#### Select

When ```select``` from a database you can add a where clause and write your condition like
you're writing an ```if``` statement in code. All you have todo is define variables of all
columns you want to use in your condition.

Once the statement is executed you get a result object. You can iterate the result with STL
iterators (iterator is a ```std::forward_iterator``` so you can only use it once).

{% highlight cpp linenos %}
matador::query<person> q("person");

matador::column name("name");
matador::column age("age");

auto res = q.select()
            .where(name != "hans" && (age > 35 && age < 45))
            .execute(conn);

for (auto item : res) {
    std::cout << "name: " << item.name << "\n";
}
{% endhighlight %}

The anonymous version works in the same way:

{% highlight cpp linenos %}
matador::query<> q;

matador::column name("name");
matador::column age("age");

auto rowres = q.select({"name", "age"})
               .from("person")
               .where(name != "hans")
               .execute(conn);

for (auto row : rowres) {
    std::cout << "name: " << row->at<std::string>("name") << "\n";
}
{% endhighlight %}

#### Delete

The ```delete``` statement works similar to the other statements. If you want to delete an
object the statement looks like this:

{% highlight cpp linenos %}
person jane("jane", 35);
matador::query<person> q("person");
// insert ...
person jane("jane", 35);
q.insert(jane).execute(conn);

matador::column name("name");
q.delete().where(name == "jane").execute(conn);
{% endhighlight %}

#### Conditions

With condition one can express a query where clause (for ```update```, ```delete``` and
```select``` queries). There are five different types of conditions:

- compare conditions
- logic conditions
- in condition
- in query condition
- range condition

##### Compare Conditions

To express a simple compare condition one needs a ```column``` object of the column to
compare. Then you can write the comparision:

{% highlight cpp linenos %}
column name("name");
name == "Jane"

column age("age");
age > 35
{% endhighlight %}

##### Logic Conditions

To concat to simple compare condition within a logical condition just write it:

{% highlight cpp linenos %}
column name("name");
column age("age");

(name != "Theo" && name != "Jane") || age > 35
{% endhighlight %}

##### IN Condition

With the IN condition one can check if a column value is one of a list.

{% highlight cpp linenos %}
column age("age");
in(age, {23,45,72});
{% endhighlight %}

##### IN Query Condition

The IN condition works also with a query.

__Note:__ You have to pass a dialect object as a third parameter to the function. You
can retrieve the dialect from the connection object.

{% highlight cpp linenos %}
column name("name");
auto q = matador::select({name}).from("test");

matador::in(name, q, &conn.dialect());
{% endhighlight %}

##### Range Condition

The range condition checks if a column value is between two given boundaries.

{% highlight cpp linenos %}
column age("age");

matador::between(age, 21, 30);
{% endhighlight %}

Take a look at the [query API reference](../api/classmatador_1_1query) to get an overview of the provided syntax.

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

## Sessions

Once the database schema is set up with the ```persistence``` object you need a [session](../api/classmatador_1_1session)
to use the ORM layer.

{% highlight cpp linenos %}
persistence p("sqlite://db.sqlite");
p.attach<person>("person");

session s(p);
{% endhighlight %}

If you have already setup the persitence layer and inserted some data, just call load to
get it into the layer.

{% highlight cpp linenos %}
// load the data from database
s.load();
{% endhighlight %}

Now you can start and insert, update or delete your data. Therefor create a transaction
object with the current session and start the transaction by calling
```matador::transaction::begin()```. After completing your modifications call
```matador::transaction::commit()``` to commit all your modifications to the
database. If in error occurred while doing your modifications catch
the exception. In the catch block you can call ```matador::transaction::rollback()```
to rollback all your modifications.

{% highlight cpp linenos %}
// create a transaction for session

matador::transaction tr = s.begin();
try {
  // begin the transaction

  // insert some objects
  s.insert(new person("joe", 45))
  s.insert(new person("walter", 56));
  s.insert(new person("helen", 37));
  s.insert(new person("tim", 14));

  // commit the modifications
  tr.commit();
} catch (exception &ex) {
  // on error rollback transactions
  tr.rollback();
}
{% endhighlight %}

## Time

matador comes with its own simple time class. It represents a time with milliseconds
precisions. Once a time object exists it can be modified, copied or assigned. For full
documentation see the [api](../api/classmatador_1_1time/).

### Creation

Time can be created from several constructors.

| Constructor | purpose |
|-------------|---------|
|```time()``` | Creates a time of now |
|```time(timt_t)```| Creates a time of ```time_t``` value|
|```time(timeval)```| Creates a time of struct ```timeval```|
|```time(int, int, int, int, int, int, long)```| Creates time from year, month, day, hour, minutes, seconds and optional milliseconds. The given parameters are validated.|

The obvious copy and assignment constructors exists as well as a static parsing function

{% highlight cpp linenos %}
matador::time t = matador::time::parse("03.04.2015 12:55:12.123", "%d.%m.%Y %H:%M:%S.%f");
{% endhighlight %}

The parse format tokens are the same as the ones from [```strptime```](https://linux.die.net/man/3/strptime)
plus the ```%f``` for the milliseconds.

### Display

The time consists of an stream output operator which displays the time in ISO8601 format

{% highlight cpp linenos %}
matador::time t(2015, 1, 31, 11, 35, 7, 123);

std::cout << t;
{% endhighlight %}

Results in:

```bash
2015-01-31T11:35:07
```

There is also a ```to_string()``` function taking the time as first parameter and a format
string as second parameter. It returns the formatted time as ```std::string```.

{% highlight cpp linenos %}
matador::time t(2015, 1, 31, 11, 35, 7, 123);

std::cout << to_string(t, "%H:%M:%S.%f %d.%m.%Y");
{% endhighlight %}

Results in:

```bash
11:35:07.123 31.01.2015
```

### Modify

To modify a time one can use the fluent interface allowing the user to concatenate
all parts to be modified in sequence.

{% highlight cpp linenos %}
matador::time t(2015, 1, 31, 11, 35, 7);
// modification
t.year(2014).month(8).day(8);
{% endhighlight %}

### Conversions

The time can be converted into a [```matador::date```](../api/classmatador_1_1date/)


{% highlight cpp linenos %}
matador::time t(2015, 1, 31, 11, 35, 7);

matador::date d = t.to_date();
{% endhighlight %}

There are also methods to retrieve ```tm``` and ```timeval``` struct:

{% highlight cpp linenos %}
matador::time t(2015, 1, 31, 11, 35, 7);

struct tm ttm = t.get_tm();
struct timeval tv = t.get_timeval();
{% endhighlight %}

## Date

matador comes with its own simple date class. It represents a date consisting of year, month
and day. The underlying calendar is a julian date calendar. Once a date object exists it
can be modified, copied or assigned. For full documentation see the [api](../api/classmatador_1_1date/).

### Creation

Date can be created from several constructors.

| Constructor | purpose |
|-------------|---------|
|```date()``` | Creates a date of now |
|```date(int)``` | Creates a date from julian date|
|```date(const char *d, const char *format)``` | Creates a date from date stamp with the given format|
|```date(int, int, int)``` | Creates a date from day, month and year|

The obvious copy and assignment constructors exists as well.

### Display

The date consists of an stream output operator which displays the date in ISO8601 format

{% highlight cpp linenos %}
matador::date d(31, 1, 2015);

std::cout << d;
{% endhighlight %}

Results in:

```bash
2015-01-31
```

There is also a ```to_string()``` function taking the date as first parameter and a format
string as second parameter. It returns the formatted date as ```std::string```.

{% highlight cpp linenos %}
matador::date d(31, 1, 2015);

std::cout << to_string(d, "%d.%m.%Y");
{% endhighlight %}

Results in:

```bash
31.01.2015
```

### Modify

To modify a date one can use the fluent interface allowing the user to concatenate
all parts to be modified in sequence.

{% highlight cpp linenos %}
matador::date d(31, 1, 2015);
// modification
d.year(2014).month(8).day(8);
{% endhighlight %}

The operators ```++```, ```--```, ```+=``` and ```-=``` are also available and increase or decrease
the date by one day or the given amount of days for the latter two operators.

{% highlight cpp linenos %}
matador::date d(31, 1, 2015);
// modification

d += 4;

std:cout << d;
{% endhighlight %}

Leads to

```bash
2015-02-04
```

### Conversions

The date can be retrieved as julian date value:

{% highlight cpp linenos %}
matador::date d(31, 1, 2015);

std::cout << "julian date: " << d.julian_date();
{% endhighlight %}

## Varchar

The ```varchar<T>``` class represents a VARCHAR database data type. Internally it contains
a ```std::string```. The template parameter takes the capacity of the varchar. If the assigned
string is exceeds the capacity, the string is truncated on database. For full documentation
see the api of [varchar_base](../api/classmatador_1_1varchar__base/) and [varchar](../api/classmatador_1_1varchar/).

### Creation

A ```varchar``` is simply created:

{% highlight cpp linenos %}
// without value
matador::varchar<255> name;

// with initial value
matador::varchar<255> name("hello world");
{% endhighlight %}

### Display

The ```varchar<T>``` has an overloaded output stream operator. So it can be written to every
stream as usual

{% highlight cpp linenos %}

matador::varchar<255> title("My title");

std::cout << "Title: " << title;
{% endhighlight %}

Leads to

```bash
Title: My title
```

### Modification

There are a few modification methods and operators like

{% highlight cpp linenos %}
matador::varchar<255> title("My title");

title += " is great";

std::cout << "Title: " << title;
{% endhighlight %}

Results in

```bash
Title: My title is great
```

Assignment is available for other ```varchar<T>``` objects, std::string and character pointers.

{% highlight cpp linenos %}
matador::varchar<255> title("My title");
matador::varchar<255> other_title("My other title");

title.assign(other_title);

std::cout << "Title: " << title;
{% endhighlight %}

Results in

```bash
Title: My other title
```

## Identifier

*Tbd!*
</div>
<div class="col-md-3 hidden-xs hidden-sm">
<nav class="bs-docs-sidebar">
* Will be replaced with the ToC, excluding the "Contents" header
{:toc .nav .nav-stacked .fixed}
</nav>
</div>
<script src="{{ "/assets/javascript/guide.js" | relative_url }}"></script>
