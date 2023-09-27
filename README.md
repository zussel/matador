matador
=======

Take your database by the horns.

[![Version 0.8.1](https://badge.fury.io/gh/zussel%2Fmatador.svg)](https://badge.fury.io/gh/zussel%2Fmatador)
[![GPLv3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://raw.githubusercontent.com/zussel/matador/develop/License)
[![C++14](https://img.shields.io/badge/language-C%2B%2B14-yellow.svg)](https://en.wikipedia.org/wiki/C%2B%2B14)

__Build Status__

| Branches    | Linux-CI | Windows-CI |  CodeCov  |
|-------------|-----------|-----------|-----------|
| master      |[![Build Status](https://github.com/zussel/matador/actions/workflows/linux.yml/badge.svg?branch=master)](https://github.com/zussel/matador/actions/workflows/linux.yml?query=branch%3Amaster)|[![Build status](https://ci.appveyor.com/api/projects/status/fs50s053d085q5j8/branch/master?svg=true)](https://ci.appveyor.com/project/zussel/matador/branch/master)|[![codecov](https://codecov.io/gh/zussel/matador/branch/master/graph/badge.svg?token=3PhtuRr0a8)](https://codecov.io/gh/zussel/matador)|
| develop     |[![Build Status](https://github.com/zussel/matador/actions/workflows/linux.yml/badge.svg?branch=develop)](https://github.com/zussel/matador/actions/workflows/linux.yml?query=branch%3Adevelop)|[![Build status](https://ci.appveyor.com/api/projects/status/fs50s053d085q5j8/branch/develop?svg=true)](https://ci.appveyor.com/project/zussel/matador/branch/develop)|[![codecov](https://codecov.io/gh/zussel/matador/branch/develop/graph/badge.svg?token=3PhtuRr0a8)](https://codecov.io/gh/zussel/matador)|

matador is an ORM and Web Application framework written in C++. It
encapsulates all database backend communication and offers a simple (web) server.
You don't have to deal with database backends or sql statements neither with mapping of
data types nor serialization of objects. It comes with relation handling out
of the box, brings a unique container for all kind of objects and a fluent sql
query interface. All features are combined into an ORM layer. With the integrated web server
it is possible to write your own web application with database backend.

Features:

 * A unique fluent query interface
 * One to one/many relations
 * One storage container
 * Filter with simple expressions
 * Transactions
 * STL like interface and iterators
 * Json support including parser and object mapper
 * Simple Logging mechanism
 * Networking
 * Web Server
 * Template Engine
 
Supported databases:
 * PostgreSQL
 * SQLite
 * MySQL
 * MS SQL Server

Documentation can be found [here](http://zussel.github.io/matador).

Example
-------

The following example define an entity _person_, sets up orm backend and
logging and creates a http server listening at port 8000. The server serves
five REST apis at ```http://localhost:8000/person/```:
- GET ```/``` (returns all person)
- GET ```/{id}``` (return person with id where id must be numeric)
- POST ```/``` (creates a person from the given json body)
- PUT ```/{id}``` (updates a person with id from the given json body)
- DELETE ```/{id}``` (deletes a person with id)

```cpp
// use matador namespace
using namespace matador

// a simple person class
struct person
{
  long id;   // primary key
  std::string name;
  date birthday;
  container<std::string> colors;
  
  person() = default;
  person(long i, std::string n)
    : id(i), name(std::move(n))
  {}
  
  template < class Operator >
  void process(Operator &op) {
    matador::access::primary_key(op, "id", id);
    matador::access::attribute(op, "name", name, 255);
    matador::access::attribute(op, "birthday", birthday);
    matador::access::container(op,              // operator
                              "person_color",  // relation table name
                              colors,          // class member
                              "person_id",     // left column in relation table
                              "color",         // right column in relation table
                              matador::cascade_type::ALL); // cascade type
  }
};

// setup application logging
add_log_sink(matador::create_file_sink("log/server.log"));
add_log_sink(matador::create_stdout_sink());

// prepare the persistence layer
persistence p("sqlite://db.sqlite");
p.enable_log();
p.attach<person>("person");

// create tables
p.create();

// create a database session and
// load data from db
session s(p);
s.load();

// initialize network stack
net::init();

// create server and add routing
http::server server(8000);
server.add_routing_middleware();

// return all persons
server.on_get("/person", [&s](const request &req) {
  auto result = s.select<person>();
  json_object_mapper mapper;

  std::string body = mapper.to_string(result, json_format::pretty);

  return response::ok(body, mime_types::TYPE_APPLICATION_JSON);
});

// return one person
server.on_get("/person/{id: \\d+}", [&s](const http::request &req) {
  auto id = std::stoul(req.path_params().at("id"));
  auto result = s.get<person>(id);

  if (result.empty()) {
    return http::response::not_found();
  }

  json_object_mapper mapper;

  std::string body = mapper.to_string(result, json_format::pretty);

  return http::response::ok(body, http::mime_types::TYPE_APPLICATION_JSON);
});

// insert person
server.on_post("/person", [&s](const http::request &req) {
  json_object_mapper mapper;

  auto p = mapper.to_ptr<person>(req.body());
  auto optr = s.insert(p.release());
  s.flush();

  std::string body = mapper.to_string(optr, json_format::pretty);
  return http::response::ok(body, http::mime_types::TYPE_APPLICATION_JSON);
});

// update person
server.on_put("/person/{id: \\d+}", [&s](const http::request &req) {
  auto id = std::stoul(req.path_params().at("id"));
  auto result = s.get<person>(id);

  if (result.empty()) {
    return http::response::not_found();
  }

  json_object_mapper mapper;
  auto p = mapper.to_ptr<person>(req.body());

  // update entity
  result.modify()->name = p->name;
  result.modify()->birthday = p->birthday;
  result.modify()->colors.clear();
  for (const auto &color : p->colors) {
    result.modify()->colors.push_back(color);
  }
  s.flush();

  return http::response::no_content();
});

// delete person
server.on_remove("/person/{id: \\d+}", [&s](const http::request &req) {
  auto id = std::stoul(req.path_params().at("id"));
  auto result = s.get<person>(id);

  if (result.empty()) {
   return http::response::not_found();
  }

  s.remove(result);

  return http::response::no_content();
});

server.run();
net::cleanup();
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
