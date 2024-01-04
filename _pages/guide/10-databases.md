## Databases

To connect to a database you have to create and open a connection. Therefor you need a
connection object. This is initialized with a connection string. The connection string commes
in an uri way:

```
<database-type>://[<username>[:<password>]@][<server>[:<port>]]/<database-name>
```
- __```database-type:```__ One of ```postgresql```, ```mysql```, ```mssql``` or ```sqlite```
- __```username:```__ The username of the database connection
- __```password:```__ The password of the database connection
- __```server:```__ The server address or hostname of the database
- __```port:```__ The server port of the database
- __```database-name:```__ The name of the database instance


__Example:__

```cpp
  connection conn("mysql://test@localhost/mydb");

  conn.open();
  // ... use connection
  conn.close();
```

### Supported Databases

There're currently four supported databases and the in memory database. Next is the description of the
database connection string for the supported databases.

#### PostgreSQL

```cpp
  // PostgreSQL connection string with password
  session ses(ostore, "postgresql://user:passwd@host/db");

  // PostgreSQL connection string without password
  session ses(ostore, "postgresql://user@host/db");
```

#### MS SQL Server

```cpp
  // MSSQL connection string with password
  session ses(ostore, "mssql://user:passwd@host/db");

  // MSSQL connection string without password
  session ses(ostore, "mssql://user@host/db");
```

#### MySQL / MariaDB

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

### Handling Database errors

Each backend has its own error handling, error codes or error messages. Some
use [SQLSTATE](https://en.wikipedia.org/wiki/SQLSTATE) other (like SQLite) don't.

Once a backend delivers a database error matador collects all available information
in a ```database_error``` object which is thrown.

```cpp
  connection conn("mysql://test@localhost/mydb");

  try {
    conn.open();
  } catch (database_error &ex) {
    std::cout << "caught error '" << ex.what() << "' from backend '" << ex.source() << "' " \
              << "with sqlstate '" << ex.sql_state() << "' and error code " << ex.error_code() << "\n";
  }
```
