---
title: Getting Started
layout: guide
permalink: /getting-started/
---
<div class="col-md-9 guide">

## Goal

The goal of this tutorial to get you up and running a simple matador executable

### Prerequistes

### Setting up entities

#### Person

```cpp
#include "matador/utils/identifier.hpp"
#include "matador/utils/varchar.hpp"
#include "matador/utils/date.hpp"

#include "matador/object/has_many.hpp"

struct person
{
  matador::identifier<unsigned long> id;
  matador::varchar<255> name;
  matador::date birthday;

  person(const std::string &n, const matador::date &bd)
    : name(n), birthday(bd)
  {}

  template < class Serializer >
  void serialize(Serializer &serializer)
  {
    serializer.serialize("id", id);
    serializer.serialize("name", name);
    serializer.serialize("birthday", birthday);
  }
};
```

#### Author

```cpp
#include "matador/object/has_many.hpp"

struct author : public person
{
  // many to one
  matador::has_many<post> posts;

  author(const std::string &name, const matador::date &birthday)
    : person(name, birthday)
  {}

  template < class Serializer >
  void serialize(Serializer &serializer)
  {
    serializer.serialize(*matador::base_class<person>(this));
    serializer.serialize("post", id, "post", "id");
  }
};
```

#### Post

```cpp
#include "matador/utils/identifier.hpp"
#include "matador/utils/varchar.hpp"
#include "matador/utils/time.hpp"

#include "matador/object/has_many.hpp"
#include "matador/object/belongs_to.hpp"

struct post
{
  matador::identifier<unsigned long> id;
  matador::varchar<255> title;
  matador::belongs_to<author> writer;
  matador::time created_at;
  matador::has_many<category> categories;
  matador::has_many<comment> comments;
  matador::has_many<varchar<255>> tags;
  std::string content;

  template < class Serializer >
  void serialize(Serializer &serializer)
  {
    serializer.serialize("id", id);
    serializer.serialize("title", title);
    serializer.serialize("author", writer, matador::cascade_type::NONE);
    serializer.serialize("created_at", created_at);
    serializer.serialize("post_category", categories, "post_id", "category_id");
    serializer.serialize("comment", comments, "comment", "id");
    serializer.serialize("post_tag", tags, "post_id", "tag");
    serializer.serialize("content", content);
  }
};
```
#### Category

```cpp
#include "matador/utils/identifier.hpp"
#include "matador/utils/varchar.hpp"

#include "matador/object/has_many.hpp"

struct category
{
  matador::identifier<unsigned long> id;
  matador::varchar<255> name;
  std::string description;
  matador::has_many<post> posts;

  template < class Serializer >
  void serialize(Serializer &serializer)
  {
    serializer.serialize("id", id);
    serializer.serialize("name", name);
    serializer.serialize("description", description);
    serializer.serialize("post_category", categories, "post_id", "category_id");
  }
};
```
#### Comment

```cpp
#include "matador/utils/identifier.hpp"
#include "matador/utils/varchar.hpp"
#include "matador/utils/time.hpp"

#include "matador/object/belongs_to.hpp"

struct comment
{
  matador::identifier<unsigned long> id;
  matador::varchar<255> email;
  matador::belongs_to<post> blog_post;
  std::string content;
  matador::time created_at;  

  template < class Serializer >
  void serialize(Serializer &serializer)
  {
    serializer.serialize("id", id);
    serializer.serialize("email", email);
    serializer.serialize("post", blog_post, matador::cascade_type::NONE);
    serializer.serialize("content", content);
    serializer.serialize("created_at", created_at);
  }
}
```

### Setting up persistence layer

```cpp
#include "matador/orm/persistence.hpp"

void setup_peristence(matador::persistence &p)
{
    p.attach<person>("person", true);
    p.attach<author, person>("author");
    p.attach<post>("post");
    p.attach<category>("category");
    p.attach<comment>("comment");
}
```

### Work with the setup

#### Create a session

```cpp
#include "matador/orm/session.hpp"

void create_session(matador::persistence &p)
{
  matador::session s(p);

  // work with the session ...
}
```

#### Add an author

```cpp
#include "matador/object/object_ptr.hpp"

matador::object_ptr<author> create_author()
{
    return s.insert(new author());
}
```

#### Add a category

```cpp
#include "matador/object/object_ptr.hpp"

matador::object_ptr<category> create_category()
{
    return s.insert(new category());
}
```

#### Add a post

```cpp
#include "matador/object/object_ptr.hpp"

matador::object_ptr<post> create_post(
    const object_ptr<author> &me,
    const object_ptr<category> &main)
{
    std::unique_ptr<post> first_post(new post);
    post->title = "My First Post";
    post->writer = me;
    post->category = main;
    post->created_at = matador::time::now();
    post->tags.push_back("c++");
    post->tags.push_back("matador");
    post->tags.push_back("tutorial");
    post->content = "This is the content of my first post";
    return s.insert(first_post.release());
}
```

</div>
<div class="col-md-3">
<nav class="bs-docs-sidebar">
* Will be replaced with the ToC, excluding the "Contents" header
{:toc .nav .nav-stacked .fixed}
</nav>
</div>
<script src="{{ base_path }}/assets/javascript/guide.js"></script>
