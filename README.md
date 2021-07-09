matador
=======

Take your database by the horns.

[![Version 0.7.0](https://badge.fury.io/gh/zussel%2Fmatador.svg)](https://badge.fury.io/gh/zussel%2Fmatador)
[![GPLv3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://raw.githubusercontent.com/zussel/matador/develop/License)
[![C++11](https://img.shields.io/badge/language-C%2B%2B11-yellow.svg)](https://en.wikipedia.org/wiki/C%2B%2B11)


__Build Status__

| Branches    | Travis-CI | AppVeyor-CI | Coveralls    |  CodeCov  |
|-------------|-----------|-------------|--------------|-----------|
| master      |[![Build Status](https://travis-ci.org/zussel/matador.png?branch=master)](https://travis-ci.org/zussel/matador?branch=master)|[![Build status](https://ci.appveyor.com/api/projects/status/fs50s053d085q5j8/branch/develop?svg=true)](https://ci.appveyor.com/project/zussel/matador/branch/master)|[![Coverage Status](https://coveralls.io/repos/github/zussel/matador/badge.svg?branch=master)](https://coveralls.io/github/zussel/matador?branch=master)|[![codecov](https://codecov.io/gh/zussel/matador/branch/master/graph/badge.svg?token=3PhtuRr0a8)](https://codecov.io/gh/zussel/matador)|
| develop     |[![Build Status](https://travis-ci.org/zussel/matador.png?branch=develop)](https://travis-ci.org/zussel/matador)|[![Build status](https://ci.appveyor.com/api/projects/status/fs50s053d085q5j8/branch/develop?svg=true)](https://ci.appveyor.com/project/zussel/matador/branch/develop)|[![Coverage Status](https://coveralls.io/repos/github/zussel/matador/badge.svg?branch=develop)](https://coveralls.io/github/zussel/matador?branch=develop)|[![codecov](https://codecov.io/gh/zussel/matador/branch/develop/graph/badge.svg?token=3PhtuRr0a8)](https://codecov.io/gh/zussel/matador)|

matador is a ORM framework written in C++. It aims to encapsulate all database stuff
(database backends, sql statements, serialization of objects) and provides the user an
easy to use interface and a unique container for all objects. Given this container the user
has a centralized point of storage for all objects at hand but with the ability to create views on
concrete object types.

Features:

 * A unique fluent query interface
 * One to one/many relations
 * One storage container
 * Filter with simple expressions
 * Transactions
 * STL like interface and iterators
 * Json support including parser and object mapper
 * Simple Logging mechanism

Supported databases:
 * PostgreSQL
 * SQLite
 * MySQL
 * MS SQL Server

Documentation can be found [here](http://zussel.github.io/matador).

Example
-------

```cpp
// use matador namespace
using namespace matador

// a simple person class
struct person
{
  identifier<long> id;   // primary key
  std::string name;
  unsigned int age = 0;
  has_many<std::string> colors;
  
  person() = default;
  person(long i, std::string n)
    : id(i), name(std::move(n))
  {}
  
  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer) {
    serializer.serialize("id", id);
    serializer.serialize("name", name, 255);
    serializer.serialize("age", age);
    serializer.serialize("person_color",  // relation table name
                         colors,          // class member
                         "person_id",     // left column in relation table
                         "color",         // right column in relation table
                         matador::cascade_type::ALL); // cascade type
  }
};

// prepare the persistence layer
persistence p("sqlite://db.sqlite");
p.attach<person>("person");

// create tables
p.create();

// create a database session
session s(p);

// insert george
// returns an matador::object_ptr<person>
auto george = s.insert(new person(1, "george"));
s.flush();

// modify george
george.modify()->age = 35;
s.save(george);

// add color
george.modify()->colors.push_back("brown");
s.save(george);

// delete george
s.remove(george);
s.flush();
```
Requirements
------------

The library has almost no dependencies. At least the database library you want to use.
If you would like to build from the sources you need at least the
[cmake](http://www.cmake.org) build system installed. If you plan to generate an install
package on a windows system you need the
[nullsoft scriptable install system](http://nsis.sourceforge.net).
          
Sources
-------

Get the sources from GitHub and enter the created directory:

    $ git clone https://github.com/zussel/matador.git
    $ cd matador

Building under Linux
--------------------

Create a build directory change to it and call cmake:

    $ mkdir build
    $ cd build
    $ cmake ..
    
Then you can build matador from sources:

    $ make

Building under Windows (for Visual Studio)
------------------------------------------

Create a build directory change to it and call cmake:

    $ mkdir build
    $ cd build
    $ cmake -G "Visual Studio *" ..

Where __*__ is one of the *__"Visual Studio"__* strings up from *__"14"__*. See cmake
documentation [here](https://cmake.org/cmake/help/v3.6/manual/cmake-generators.7.html?#visual-studio-generators).
After generation you find a __matador.sln__ solution file in the current directory.

Contact
-------

If you have questions or issues concerning matador you can place an issue in my
[matador github repository](https://github.com/zussel/matador/issues?milestone=1&state=open)
