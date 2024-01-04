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
You can use an anonymous query as well. This will use an object
```matador::row``` to store the result. A can be build up by addiing
several columns. This can be done programatically using ```add_columns(...)``` or
in the case below determined by a query.

{% highlight cpp linenos %}
    matador::query<> q;
    auto res = count
        .select({columns::count_all()})
        .from("person")
        .execute(*conn);

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
        make_pk_column<long>("id"),
        make_column<std::string>("name", 255),
        make_column<unsigned>("age")
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
    matador::query<> person_query("person");

    person_query
        .insert({"id", "name", "age"})
        .values({1, "jane", 35})
        .execute(conn);
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

    q.update(jane).where("name"_col == "jane").execute(conn);
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

    auto name = "name"_col;
    auto age = "age"_col;

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

    auto rowres = q.select({"name", "age"})
                .from("person")
                .where("name"_col != "hans")
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

    q.delete().where("name"_col == "jane").execute(conn);
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

##### Like Condition

To express a _like_ condition one has to call ```like``` with the column and
the compare value.

{% highlight cpp linenos %}
    auto name = "name"_col;

    matador::like(name, "%ight%");
{% endhighlight %}

##### Range Condition

The range condition checks if a column value is between two given boundaries.

{% highlight cpp linenos %}
    column age("age");

    matador::between(age, 21, 30);
{% endhighlight %}

Take a look at the [query API reference](../api/classmatador_1_1query) to get an overview of the provided syntax.
