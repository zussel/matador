---
title: Installation
---
There are almost no dependencies. At least the database library you want to use.
If you wnat to build from the sources you need an appropiate compiler
(gcc or clang) and the [cmake](http://www.cmake.org) build system installed.
If you plan to generate an install package on a windows system you need
the [nullsoft scriptable install system](http://nsis.sourceforge.net).

### Binary Packages

<script src="{{ "/assets/javascript/downloads.js" | relative_url }}"></script>
<table class="table table-bordered table-striped" id="downloads">
  <thead>
    <tr>
      <th>File</th><th>Size (bytes)</th>
    </tr>
  </thead>
  <tbody></tbody>
</table>

Download the appropriate package and install it on your system. Once installed you
can start developing your app starting with the [introduction](/docs/introduction).

### Sources

Get the sources from GitHub and enter the created directory:

```bash
$ git clone https://github.com/zussel/matador.git
$ cd matador
```

Or get the pre-packaged sources:

[zip](https://github.com/zussel/matador/archive/v0.2.1.zip){: .btn .btn-success} [tar.gz](https://github.com/zussel/matador/archive/v0.2.1.tar.gz){: .btn .btn-success}

### Building under Linux

Create a build directory change to it and call cmake:

```bash
$ mkdir build
$ cd build
$ cmake ..
```

Then you can build matador from sources:

```bash
$ make
```

### Building under Windows (for Visual Studio)

Create a build directory change to it and call cmake:

```bash
$ mkdir build
$ cd build
$ cmake -G "Visual Studio *" ..
```

Where `*` is one of the `Visual Studio` strings up from `14`. See cmake
documentation [here](https://cmake.org/cmake/help/v3.6/manual/cmake-generators.7.html?#visual-studio-generators).
After generation you find a `matador.sln` solution file in the current directory.
