---
title: Relations
---
{% include base_path %}
{% include toc icon="columns" title="Relations" %}

When it comes to object relations you can use one to one and
one to many relations in a straight forward way. Therefor object store provides
easy wrapper for these relations

**Node:** As you can see ```many to many``` is missing, but it is the next feature to
be implemented.
{: .notice--warning}

### OneToOne Relations

In this example we have two object types an address class
and a person class.

```cpp
struct address
{
  std::string street;
  std::string city;
  
  // implement proper serializer
};
```

Each person class should have an address, so we add an
```oos::has_one``` of type address to the members. That's
it. Now we have a one to one relationship beetween two
classes.

```cpp
struct person
{
  // ...
  oos::hans_one<address> addr;
  
  template < class SERAILIZER >
  void serialize(SERAILIZER &serailizer)
  {
    // ...
    serialize("address", addr, cascade_type::ALL);
  }
};
```

With this kind of relationship we have a hard linked
relationship. Which means if we remove the person from
our store the address object is removed as well. The
```cascade_type::ALL``` means the all operation of
```INSERT```, ```UPDATE``` and ```DELETE``` will take
affect on the member address as well

So in case we want to keep the address object we can use
a ```cascade_type::NONE``` in the serialize method (this
is the default).

```cpp
  template < class SERAILIZER >
  void serialize(SERAILIZER &serailizer)
  {
    // ...
    serialize("address", addr, cascade_type::NONE);
  }
```

With ```cascade_type::NONE``` we have a kind of soft link to
the address inside our person class and it won't be removed
on a person removal. We must explicitly remove the address.

### OneToMany Relations

When it comes to one to many relationships it is also
quiet easy. OOS comes with a handy wrapper to the STL container
class ```std::vector``` and ```std::list```.

**Node:** The STL container class ```std::set``` and ```std::unordered_set```
will be supported soon.
{: .notice--warning}

Because these classes are designed in the same way as
the STL classes we can use them in the same way plus the
benefit of the relationship.

Our handy person class needs a list of friends which are also
of type person. And because we want this list with soft linked
person we use oos::object_ref as the type of the list.

```cpp
struct person
{
  std::string name;
  oos::has_many<person> friends;
  
  // c'tor for person
  person(const std::string &n) : name(n) {}
};
```

Why declare a list if we don't use it? Next is an example howto
to use our persons friend list.

We insert a new person into the object_store. Than we insert and
immediately add some persons as friends to our first person.

```cpp
// ...
// create a new person
auto joe = new person("joe");

// add some friends 
joe->push_back(new person("walter"));
joe->push_back(new person("helen"));
joe->push_back(new person("tim"));

// insert joe
auto joeptr = ostore.insert(joe);
```

Now we can simply iterate over the list like we used to
do it with all STL containers. Period.

```cpp
// access all friends
auto i = joeptr->friends->begin();
for (i; i != p->end(); ++i) {
  std::cout << "friend: " << i->name << std::endl;
}
```
