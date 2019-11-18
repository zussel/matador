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

Now you can start and insert, update or delete your data. This can be done in two ways. Using ```session::save()``` or ```session::flush()``` to write the changes directly to database or use a transactional scope to allow a rollback of a transaction.

Using the direct way it looks like the following code:
{% highlight cpp linenos %}
// insert and save an object directly
auto ptr = s.save(new person("james bond"));

// do multiple modifications and flush them at once
ptr.modify()->name = "james blunt"

auto addr = s.insert(new address("downing street 10"));

// flush changes
s.flush();
{% endhighlight %}


When using the transactional way you have to create a instance of ```transaction``` with the current session and start the transaction by calling
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
