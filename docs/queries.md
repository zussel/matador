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
oos::query<> q;
// Todo: implement functionality
```

#### Drop

The ```drop``` method is used to drop a table. The typed usage is as follows:

```cpp
oos::query<person> q("person");

// create the table based on the given type
q.drop().execute(conn);
```

#### Insert

#### Update

#### Select

#### Delete

Take a look at the [query API reference](/#) to get an overview of the provided syntax.
