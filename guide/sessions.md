---
title: Sessions
---
Once the database schema is set up with the ```persistence``` object you need a session
to use the ORM layer.

```cpp
persistence p("sqlite://db.sqlite");
p.attach<person>("person");

session s(p);
```

If you have already setup the persitence layer and inserted some data, just call load to
get it into the layer.

```cpp
// load the data from database
s.load();
```

Now you can start and insert, update or delete your data. Therefor create a transaction
object with the current session and start the transaction by calling
```matador::transaction::begin()```. After completing your modifications call
```matador::transaction::commit()``` to commit all your modifications to the
database. If in error occurred while doing your modifications catch
the exception. In the catch block you can call ```matador::transaction::rollback()```
to rollback all your modifications.

```cpp
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
```
