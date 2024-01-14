## matador 0.9.2 "Homerian"

This is a bugfix release. The following issues will be addressed
- Raise exception if a specific column value is null (mysql query) [#156](https://github.com/zussel/matador/issues/156)

  Will fix an additional bug concerning prepared statements for mysql. 

Look [here](https://en.wikipedia.org/wiki/Homerian) to find out what Homerian is.

## matador 0.9.1 "Sheinwoodian"

This is a bugfix release. The following issues will be addressed
- Raise exception if a specific column value is null (mysql query) [#156](https://github.com/zussel/matador/issues/156)

  Will fix the same issue for mssql/odbc 

Look [here](https://en.wikipedia.org/wiki/Sheinwoodian) to find out what Sheinwoodian is.

## matador 0.9.0 "Telychian"

**Breaking Changes:** This release changes the serialization interface going from a generic
```serialize(...)``` naming to a more explicit naming. Now matador will have
- "on_primary_key(...)"
- "on_attribute(...)"
- "on_belongs_to(...)"
- "on_has_one(...)"
- "on_has_many(...)"
  for the serialization interface.

This leads to a more readable database entity description and the remove of the
```identifier<Type>```, ```has_one<Type>``` and ```belongs_to<Type>``` classes. For
primary key the plain datatype (integral and sized string -> varchar) can be used.
For foreign object only ```object_ptr<Type>``` is used.

Furthermore, the interface is extended with a ```field_attributes``` parameter. This
class contains constraints like **NOT_NULL**, **PRIMARY_KEY** or **UNIQUE** and a size
parameter used for varchar (through std::string)

Look [here](https://en.wikipedia.org/wiki/Telychian) to find out what Telychian is.

## matador 0.8.1 "Aeronian"

Refactor reactor to process incoming request in a multithreading way. [#130](https://github.com/zussel/matador/issues/130)
The implemented pattern is the leader follower pattern.

Look [here](https://en.wikipedia.org/wiki/Aeronian) to find out what Aeronian is.

## matador 0.8.0 "Rhuddanian"

This release comes with a bunch of changes:
 - A network module implementing the reactor pattern (only single threaded by now)
 - An HTTP server and client based on the network module
     - The HTTP server implements the middleware pattern
     - A template engine not only for the HTTP server
 - Handle collection with a streaming interface
 - Better JSON object mapper
 - And of course bugfixes

Look [here](https://en.wikipedia.org/wiki/Rhuddanian) to find out what Rhuddanian is.

## matador 0.7.0 "Hirnantian"

This release will mainly contain a simple JSON class coming with a json class, a parser and an object mapper and a simple logging mechanism. As usual some bugs will be fixed as well.

- JSON module [#115](https://github.com/zussel/matador/issues/115)
- Logging module [#121](https://github.com/zussel/matador/issues/121)
- Bugfix: result::empty() always returned ```false``` [#118](https://github.com/zussel/matador/issues/118)

Look [here](https://en.wikipedia.org/wiki/Hirnantian) to find out what Hirnantian is.

## matador 0.6.2 "Katian"

This is a bugfix release. The following issues will be addressed

 - Handle of removed my_bool in mysql 8 [#110](https://github.com/zussel/matador/issues/110)
 - Route all db error information through an exception [#107](https://github.com/zussel/matador/issues/107)
 - Fix mysql bug [#111](https://github.com/zussel/matador/issues/111)
 - Introduce like() command for queries [#109](https://github.com/zussel/matador/issues/109)

Look [here](https://en.wikipedia.org/wiki/Katian) to find out what Katian is.

## matador 0.6.1 "Sandbian"

This release focused on some bugfixes and enhancements according to the supported
datatype list. Furthermore, the session class is now more intuitive and the documentation
was updated.

 - Make session class more intuitive [#87](https://github.com/zussel/matador/issues/87)
 - Increase stability for SQLite backend [#91](https://github.com/zussel/matador/issues/91)
 - Removed varchar class (use it only as a container helper) [#104](https://github.com/zussel/matador/issues/104)
 - Handle int64/uint64 values [#106](https://github.com/zussel/matador/issues/106)
 - Update documentation [#103](https://github.com/zussel/matador/issues/103)

Look [here](https://en.wikipedia.org/wiki/Sandbian) to find out what Sandbian is.

## matador 0.6.0 "Darriwilian"

This release adds support for [PostgreSQL](https://github.com/zussel/matador/projects/1) database.

Look [here](http://en.wikipedia.org/wiki/Darriwilian) to find out what Darriwilian is.

## matador 0.5.1 "Dapingian"

This feature solves an issue when deleting objects within a container. Furthermore, a lot of CMake issues were solved.

Here are the changes in detail:

 - Build for VS2017 [#89](https://github.com/zussel/matador/issues/89)
 - Improve deletion of objects with containers containing objects [#88](https://github.com/zussel/matador/issues/88)
 - Generate Windows ZIP package on build [#85](https://github.com/zussel/matador/issues/85)
 - Generate ArchLinux package on build [#84](https://github.com/zussel/matador/issues/84)
 - Generate RPM package on build [#83](https://github.com/zussel/matador/issues/83)
 - Generate Debian package on build [#82](https://github.com/zussel/matador/issues/82)
 - Move tables to persistence class [#86](https://github.com/zussel/matador/issues/86)

Look [here](http://en.wikipedia.org/wiki/Dapingian) to find out what Dapingian is.

## matador 0.5.0 "Floian"

This release features major changes how matador deals with objects/entities. Furthermore,
real relationship handling was implemented. And at least the name of the library changed
from OOS to matador.

Here are the changes in detail:

 - No need to derive from a base class anymore. Object classes can be at least a pod. (#48)
 - Real relationship handling (#74, #45)
 - Stabilize SQL query module (#62, #64, #67, #47, #49)
 - Add time and date class (#12)
 - Improved project structure (#63, #60, #55)
 - Many little other improvements and bugfixes
 - And finally changed the project name to __matador__

Look [here](http://en.wikipedia.org/wiki/Floian) to find out what Floian is.

## oos 0.2.1 "Tremadocian"

This is a bugfix release consisting of the following fixes:

 - Fixed build failure under ubuntu 13.10 (thanx to [TyRoXx](https://github.com/TyRoXx))
 - Replaced exits with throws
 - Removed all debug __couts__

Look [here](http://en.wikipedia.org/wiki/Tremadocian) to find out what _Tremadocian_ is.

## oos 0.2.0 "Furongian"

This release features the database backend for MS SQL Server. Here is the complete list
of changes:

 - Support of MS SQL Server
 - Bugfix in MySQL backend
 - Performance improvement for MySQL backend
 
Look [here](http://en.wikipedia.org/wiki/Furongian) to find out what _Furongian_ is.

## oos 0.1.0 "Terreneuvium"

This is the initial release of Open Object Store (oos). oos is an c++ orm framework.
As a unique feature it comes with a single point storage for all kind of objects
called object store. Furthermore, oos has the following main features:

 - Encapsulate all SQL related things (datatype mapping, serialization etc.)
 - STL like interface
 - Transaction mechanism
 - Internal reference counting mechanism
 - Support for __MySQL__ and __SQLite__
 - Default in-memory database
 - Helpful utils: unit test classes, json de/serializer, data type convert functions, etc.

Look [here](http://en.wikipedia.org/wiki/Terreneuvian) to find out what _Terreneuvium_ is.
