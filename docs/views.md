---
title: Views
---
{% include base_path %}

In most cases we want to iterate over all objects of a certain
type. How can we achieve this? We open a view for the type we want
to inspect.

Again we use our little person class and insert some persons into
our store.

```cpp
// add some friends
ostore.insert(new person("joe"))
ostore.insert(new person("walter"));
ostore.insert(new person("helen"));
ostore.insert(new person("tim"));
```

Than we create a view with the ```oos::object_view``` class. This class
takes as the template parameter the desired class type. Then we can
use the view like a STL list containing ```oos::object_ptr``` of our
desired type.

```cpp
// shortcut to the person view
typedef oos::object_view<person> person_view_t;

person_view_t pview(ostore);

person_view_t::iterator i = pview.begin();
for (i; i != pview.end(); ++i) {
  std::cout << "person: " << i->name << std::endl;
}
```

But this class can to somethig more for us. If we have a hierarchy
of classes, we can create a view of the base type and easily iterate
overall sub-types.

Look at the next example. We have a hierarchy with the person class
as base class and inherited from that we have classes student and
employee.
And again we create a view of type person to access all objects of
type person including sub-types student and employee.

```cpp
class student : public person { //... };
class employee : public person { //... };

oos::object_store ostore;
ostore.attach<person>("person");
ostore.attach<student, person>("student");
ostore.attach<employee, person>("employee");

typedef oos::object_view<person> person_view_t;

person_view_t pview(ostore);

person_view_t::iterator i = pview.begin();
for (i; i != pview.end(); ++i) {
  std::cout << i->name() << std::endl;
}
```
