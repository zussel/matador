Open Object Store (OOS)
=======================

__Version 0.5.0-alpha.1 (License [GPLv3](http://www.gnu.org/licenses/gpl.html))__

Store all kind of objects in one container.

Build status at [travis-ci](https://travis-ci.org),
Code coverage at [coveralls.io](https://coveralls.io)

[![Build Status](https://travis-ci.org/zussel/oos.png?branch=develop)](https://travis-ci.org/zussel/oos)
[![Build Status](https://ci.appveyor.com/api/projects/status/github/zussel/oos?svg=true)](https://ci.appveyor.com/project/zussel/oos)
[![Coverage Status](https://coveralls.io/repos/zussel/oos/badge.png?branch=develop)](https://coveralls.io/r/zussel/oos?branch=develop)
[![Ohloh](http://www.ohloh.net/p/openobjectstore/widgets/project_thin_badge.gif)](https://www.ohloh.net/p/openobjectstore)


OOS is a ORM framework written in C++. It aims to encapsulate all database stuff
(database backends, sql statements, serialization of objects) and provides the user an
easy to use interface and a unique container for all objects. Given this container the user
has a centralized point of storage for all objects at hand but with the ability to create views on
concrete object types.

Features:

 * Encapsulate database backends
 * Support for SQLite, MySQL and MS SQL Server
 * Encapsulate sql statements
 * Fluent sql query builder
 * One to one/many relations
 * One storage container
 * Filter with simple expressions
 * Transactions
 * STL like interface and iterators

Documentation can be found [here](http://zussel.github.com/oos).

Requirements
------------

The library has very less dependencies. At least the database library you want to use
in your environments path. If you would like to build from the sources you need at least the
[cmake](http://www.cmake.org) build system installed. If you plan to generate an install
package on a windows system you need the
[nullsoft scriptable install system](http://nsis.sourceforge.net).
          
Sources
-------

Get the sources from GitHub and enter the created directory:

    $ git clone https://github.com/zussel/oos.git
    $ cd oos

Building under Linux
--------------------

Create a build directory change to it and call cmake:

    $ mkdir build
    $ cd build
    $ cmake ..
    
Then you can build oos from sources:

    $ make

Building under Windows (for Visual Studio)
------------------------------------------

Create a build directory change to it and call cmake:

    $ mkdir build
    $ cd build
    $ cmake -G "Visual Studio *" ..

Where __*__ is one of the *__"Visual Studio"__* strings up from *__"14"__*. See cmake
documentation [here](https://cmake.org/cmake/help/v3.6/manual/cmake-generators.7.html?#visual-studio-generators).
After generation you find a __oos.sln__ solution file in the current directory.

Contact
-------

If you have questions or issues concerning oos you can place an issue in my
[oos github repository](https://github.com/zussel/oos/issues?milestone=1&state=open) or
contact me via mail *__oos at zussel dot de__*.
