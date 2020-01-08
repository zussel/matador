## matador 0.6.2 "Katian"

This is a bugfix release. The following issues will be addressed

 - Handle of removed my_bool in mysql 8 [#110](https://github.com/zussel/matador/issues/110)
 - Route all db error information through an exception [#107](https://github.com/zussel/matador/issues/107)
 - Fix mysql bug [#111](https://github.com/zussel/matador/issues/111)
 - Introduce like() command for queries [#109](https://github.com/zussel/matador/issues/109)

Look [here](https://en.wikipedia.org/wiki/Katian) to find out what Katian is.

## matador 0.6.1 "Sandbian"

This release focused on some bugfixes and enhancements according to the supported
datatypes. Furthermore the session class is now more intuitive and the documentation
was updated.

 - Make session class more intuitive [#87](https://github.com/zussel/matador/issues/87)
 - Increase stability for SQLite backend [#91](https://github.com/zussel/matador/issues/91)
 - Removed varchar class (use it only as a has_many helper) [#104](https://github.com/zussel/matador/issues/104)
 - Handle int64/uint64 values [#106](https://github.com/zussel/matador/issues/106)
 - Update documentation [#103](https://github.com/zussel/matador/issues/103)

Look [here](https://en.wikipedia.org/wiki/Sandbian) to find out what Sandbian is.

## matador 0.6.0 "Darriwilian"

This release adds support for [PostgreSQL](https://github.com/zussel/matador/projects/1) database.

Look [here](http://en.wikipedia.org/wiki/Darriwilian) to find out what Darriwilian is.

## matador 0.5.1 "Dapingian"

This feature solves an issue when deleting objects within a container. Furthermore a lot of CMake issues were solved.

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

This release features major changes how matador deals with objects/entities. Furthermore
real relationship handling was implemented. And at least the name of the library changed
from OOS to matador.

Here are the changes in detail:

 - No need to derive from a base class anymore. Objects classes can be at least a pod. (#48)
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
 - Removed all debug couts

Look [here](http://en.wikipedia.org/wiki/Tremadocian) to find out what _Tremadocian_ is.

## oos 0.2.0 "Furongian"

This release features the database backend for MS SQL Server. Here is the complete list
of changes:

 - Support of MS SQL Server
 - Bugfix in MySQL backend
 - Performance improvment for MySQL backend
 
Look [here](http://en.wikipedia.org/wiki/Furongian) to find out what _Furongian_ is.

## oos 0.1.0 "Terreneuvium"

This is the initial release of Open Object Store (oos). oos is an c++ orm framework.
As a unique feature it comes with a single point storage for all kind of objects
called object store. Furthermore oos has the following main features:

 - Encapsulate all SQL related things (datatype mapping, serializationm etc.)
 - STL like interface
 - Transaction mechanism
 - Internal reference counting mechanism
 - Support for __MySQL__ and __SQLite__
 - Default in-memory database
 - Helpful utils: unit test classes, json de/serializer, data type convert functions, etc.

Look [here](http://en.wikipedia.org/wiki/Terreneuvian) to find out what _Terreneuvium_ is.
