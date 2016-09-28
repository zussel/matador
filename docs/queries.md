---
title: Queries
---
{% include base_path %}
{% include toc icon="columns" title="Query Commands" %}

On the low level side of the library resides the fluent query interface. It allows you
to write a SQL query in safe way without concerning about the current SQL dialect. There
are two type of queries a typed one and an anonymous one dealing with a ```row``` object.

Once you have established a connection to yout database you can execute a query.
 
```cpp
oos::connection conn("sqlite://test.sqlite");

conn.open();
// create a typed query
oos::query<person> q("person");

// create the table based on the given type
q.create().execute(conn);
```
You can use an anonymous query as well:

```cpp

oos::query<> q;
auto res = count.select({columns::count_all()}).from("person").execute(*conn);

int count = res.begin()->at<int>(0); 
```

**Note:** The query interface represents not the full command syntax. By now it provides
basic functionality to ```create```, ```drop```, ```insert```, ```update```, ```select```
and ```delete``` a table.
{: .notice--warning}

#### Create

The ```create``` method is used to create a database table. The typed version looks
like this:

```cpp
oos::query<person> q("person");

// create the table based on the given type
q.create().execute(conn);
```

When using the anonymous version you have to describe the fields of the table to create:

```cpp
oos::query<> q("person");
// Todo: implement functionality
q.create({
    make_typed_id_column<long>("id"),
    make_typed_varchar_column<std::string>("name"),
    make_typed_column<unsigned>("age")
}).execute(conn);
```

#### Drop

The ```drop``` method is used to drop a table. The typed usage is as follows:

```cpp
oos::query<person> q("person");

// create the table based on the given type
q.drop().execute(conn);
```
The anonymous version is like this:

```cpp
oos::query<> q;

// create the table based on the given type
q.drop("person").execute(conn);
```

#### Insert

Inserting an object is done as simple. Just pass the object to the ```insert``` method
and you're done.

```cpp
oos::query<person> q("person");

person jane("jane", 35);

q.insert(&jane).execute(conn);
```

When building an anonymous insert statement one has to column and value fields like that

```cpp
oos::query<> q("person");

q.insert({"id", "name", "age"}).values({1, "jane", 35}).execute(conn);
```

#### Update

Updating an object works the same way as inserting an object. Asume there is an object
one can modify it and pass it to the ```update``` method. Notice the where clause with
expression to limit the update statement. These conditions are explained [condition chapter](/#) bewlow

```cpp
person jane("jane", 35);
oos::query<person> q("person");
// insert ...
jane.age = 47;
oos::column name("name");

q.update().set(&jane).where(name == "jane").execute(conn);
```

If you're dealing with an anonymous row query the update query looks like the example below. As
you can see, it is simple done with initializer list and pairs of columns and their new
values.

```cpp
oos::query<> q("person");

oos::column name("name");
q.update({
    {"name", "otto"},
    {"age", 47}
}).where(name == "jane");

```
#### Select

#### Delete

#### Conditions

Take a look at the [query API reference](/#) to get an overview of the provided syntax.
