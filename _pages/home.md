---
layout: splash
excerpt: 'OOS is an object-relational mapping (ORM) framework written in C++. It aims to encapsulate all the database backend stuff. You dont have to deal with database backends or sql statements neither with mapping of data types or serialization of objects. It comes with a unique container for your objects, a fluent sql query interface and on top an ORM layer.<br /> <small><a href="https://github.com/zussel/oos/releases/tag/v0.2.1">Latest release v0.2.1</a></small><br /><br /> {::nomarkdown}<iframe style="display: inline-block;" src="https://ghbtns.com/github-btn.html?user=zussel&repo=oos&type=star&count=true&size=large" frameborder="0" scrolling="0" width="160px" height="30px"></iframe> <iframe style="display: inline-block;" src="https://ghbtns.com/github-btn.html?user=zussel&repo=oos&type=fork&count=true&size=large" frameborder="0" scrolling="0" width="158px" height="30px"></iframe>{:/nomarkdown}'
permalink: /
header:
  overlay_image: library-header.png
  overlay_filter: 0.5
  cta_label: "<i class='fa fa-download'></i> Get it now"
  cta_url: "/docs/installation/"
feature_row:
  - title: "Unique Object Container"
    image_path: container400x230.png
    excerpt: |
        OOS comes with a unique kind of container where you can store all
        your objects in. Once the hierarchy is set up you can perform all
        CRUD operations on the container plus filtering and using views.
    url: "/docs/objects/"
    btn_label: "Learn More"
  - title: "Fluent Query Interface"
    image_path: fluent-road400x230.png
    excerpt: |
        The sql query module provides simple fluent
        interface to build sql queries. Once a query
        is created it can be executed directly or create
        a prepared statement.
    url: "/docs/queries/"
    btn_label: "Learn More"
  - title: "ORM Layer"
    image_path: gears400x230.png
    excerpt: |
        The ORM layer let the user set up the object hierarchy
        and hides then all the sql query stuff behind a simple
        ORM interface. All actions are executed in transaction
        scope. 
    url: "/docs/persistence/"
    btn_label: "Learn More"
---

{% include feature_row id="feature_row" %}

### Getting Started

