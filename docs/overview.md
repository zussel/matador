---
title: "Overview"
modified: 2016-09-16T13:33:02+02:00
---

{% include base_path %}

The following documentation will give you an overview on Open Object Store. It will show you all main components of the library so that you can start building your own application with it.

OOS consists of three main parts: A container for any kind of objects, a sql query class providing a fluent interface and on top a ORM layer.

### The object store

The [object store](/#) is the central element. Once it is configured with an object hierarchy, one can insert, update or delete any of objects of the introduced types. Open a view to access all objects of a specific type or of a specific base type. Once the [view](/#) is created all objects can be iterated or filtered via a simple [expression](/#). As a special feature a [transaction](/#) mechanism is also integrated.

### The SQL Query

