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

q.insert(jane).execute(conn);
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

q.update(jane).where(name == "jane").execute(conn);
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

When ```select``` from a database you can add a where clause and write your condition like
you're writing an ```if``` statement in code. All you have todo is define variables of all
columns you want to use in your condition.

Once the statement is executed you get a result object. You can iterate the result with STL
iterators (iterator is a ```std::forward_iterator``` so you can only use it once).

```cpp
oos::query<person> q("person");

oos::column name("name");
oos::column age("age");

auto res = q.select()
            .where(name != "hans" && (age > 35 && age < 45))
            .execute(conn);

for (auto item : res) {
    std::cout << "name: " << item.name << "\n";
}
```

The anonymous version works in the same way:

```cpp
oos::query<> q;

oos::column name("name");
oos::column age("age");

auto rowres = q.select({"name", "age"})
               .from("person")
               .where(name != "hans")
               .execute(conn);

for (auto row : rowres) {
    std::cout << "name: " << row->at<std::string>("name") << "\n";
}
```

#### Delete

The ```delete``` statement works similar to the other statements. If you want to delete an
object the statement looks like this:

```cpp
person jane("jane", 35);
oos::query<person> q("person");
// insert ...
person jane("jane", 35);
q.insert(jane).execute(conn);

oos::column name("name");
q.delete().where(name == "jane").execute(conn);
```

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

```cpp
column name("name");
name == "Jane"

column age("age");
age > 35
```

##### Logic Conditions

To concat to simple compare condition within a logical condition just write it:

```cpp
column name("name");
column age("age");

(name != "Theo" && name != "Jane") || age > 35
```

##### IN Condition

With the IN condition one can check if a column value is one of a list.

```cpp
column age("age");
in(age, {23,45,72});
```

##### IN Query Condition

The IN condition works also with a query.

__Note:__ You have to pass a dialect object as a third parameter to the function. You
can retrieve the dialect from the connection object.

```cpp
column name("name");
auto q = oos::select({name}).from("test");

oos::in(name, q, &conn.dialect());
```

##### Range Condition

The range condition checks if a column value is between two given boundaries.

```cpp
column age("age");

oos::between(age, 21, 30);
```

Take a look at the [query API reference](/#) to get an overview of the provided syntax.
