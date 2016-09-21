---
title: Databases
---
{% include base_path %}
{% include toc icon="columns" title="Object Store" %}

When it comes to persistence you have to deal with sessions
and transactions. You have to create a session for a specific
database and wrap a transaction around your insertions or
modifications.

After created a session objects you can either create the
database schema ...

```cpp
// the object store
object_store ostore;

// configure object store
// ...

// create and open a session
oos::session ses(ostore, "sqlite://person.db");

// create the database schema
ses.create();
```

... or load the data from database.

```cpp
// load the data from database
ses.load();
```

Once the schema is created or the data is loaded you can start
and insert, update or delete your data. Therefor create a transaction
object with the current session and start the transaction by calling
```oos::transaction::begin()```. After completing your modifications call
```oos::transaction::commit()``` to commit all your modifications to the
database. If in error occurred while doing your modifications catch
the exception. In the catch block you can call ```oos::transaction::rollback()```
to rollback all your modifications.

```cpp
// create a transaction for session

oos::transaction tr(ses);
try {
  // begin the transaction
  tr.begin();

  // insert some objects
  ostore.insert(new person("joe", 45))
  ostore.insert(new person("walter", 56));
  ostore.insert(new person("helen", 37));
  ostore.insert(new person("tim", 14));

  // commit the modifications
  tr.commit();
} catch (exception &ex) {
  // on error rollback transactions
  tr.rollback();
}
```

### Supported Databases

There're currently three supported databases and the in memory database.
But more databases will follow. Next is the description of the
database connection string for the supported databases.

#### MS SQL Server

```cpp
// MSSQL connection string with password
session ses(ostore, "mssql://user:passwd@host/db");

// MSSQL connection string without password
session ses(ostore, "mssql://user@host/db");
```

#### MySQL

```cpp
// MySQL connection string with password
session ses(ostore, "mysql://user:passwd@host/db");

// MySQL connection string without password
session ses(ostore, "mysql://user@host/db");
```

#### SQLite

```cpp
// MySQL connection string
session ses(ostore, "sqlite://database.sqlite");
```
