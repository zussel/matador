---
layout: guide
title: Databases
---

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

```cpp
connection conn("mysql://test@localhost/mydb");

conn.open();
// ... use connection
conn.close();
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
