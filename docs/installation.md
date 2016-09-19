---
title: "Installation"
---

{% include base_path %}

### Requirements

There are almost no dependencies. At least the database library you want to use. If you wnat to build from the sources you need an appropiate compiler (gcc or clang) and the [cmake](http://www.cmake.org) build system installed. If you plan to generate an install package on a windows system you need the [nullsoft scriptable install system](http://nsis.sourceforge.net).
          
### Sources

Get the sources from GitHub and enter the created directory:

```
    $ git clone https://github.com/zussel/oos.git
    $ cd oos
```

### Building under Linux
--------------------

Create a build directory change to it and call cmake:

```
$ mkdir build
$ cd build
$ cmake ..
```
    
Then you can build oos from sources:

```
$ make
```

### Building under Windows (for Visual Studio)

Create a build directory change to it and call cmake:

```
$ mkdir build
$ cd build
$ cmake -G "Visual Studio *" ..
```

Where `*` is one of the `Visual Studio` strings up from `14`. See cmake
documentation [here](https://cmake.org/cmake/help/v3.6/manual/cmake-generators.7.html?#visual-studio-generators).
After generation you find a `oos.sln` solution file in the current directory.
