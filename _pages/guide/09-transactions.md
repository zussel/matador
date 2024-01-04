## Transactions

A handy feature ist the transaction mechanism provided by the ```matador::object_store```. Once an
object is inserted, updated or deleted the change is not reversible. If you use a transaction
it is. Therefor a transaction must be started:

{% highlight cpp linenos %}
  matador::object_store store;
  store.attach<person>("person");

  matador::transaction tr(store);

  try {
    tr.begin();

    auto hans = store.insert<person>("Hans");

    /// do some stuff and commit
    tr.commit();
  } catch (std::exception &) {
    tr.rollback();
  }
{% endhighlight %}

Once a transaction is started all changed are tracked until a ```commit``` is called. When it
comes to rollback a transaction ```rollback``` must be called (i.e an exception is thrown).

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
