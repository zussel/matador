---
title: "About"
layout: single
excerpt: "Why I wrote this library"
permalink: /about/
---
**What?**

OOS is an object-relational mapping (ORM) framework written in C++. It aims to
encapsulate all the database work. You don't have to deal with database backends
or sql statements neither with serialization of objects. Its future goals are to
cover more databases and add more (hibernate like) functionality.

**Why?**

Why did I wrote this library? This question has several answers. I'll try to cover
them all in a short way: There are a some very good C++ ORM libraries out there like
[ODB](http://www.codesynthesis.com/products/odb/), [SOCI](http://soci.sourceforge.net/)
or [YB.ORM](https://code.google.com/p/yb-orm/). Although they are all excellent and
very well written I missed at least one part.

There was never a container for my objects. I always had to setup a storage point
(list, map, vector, etc) by myself. I had to deal with several containers. But what
I wanted was one container. In 2011 I started to create such a container from scratch
for two reasons: To have such a container and to use this container and to study how
this container could work and integrate into an ORM system. I tried to keep always the
user in mind wanting to work with this library. So I kept the API simple wherever I
could.

Right now it works very well. As a prove of concept I added a transaction mechanism
and three database backends.
