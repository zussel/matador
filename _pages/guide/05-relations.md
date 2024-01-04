## Relations

When it comes to object relations you can use one to one, one to many and
many to many relations in a straight forward way. Therefor object store provides
easy wrapper for these relations

### OneToOne Relations

In this example we have two object types an address class
and a person class. The address class acts as the ```belongs_to``` part
of the relation. So we add a ```matador::object_ptr<person>``` to our address
class. Don't forget to add the appropriate ```belongs_to()``` to the
```process()``` method.

For the person in address we add the ```matador::cascade_type::NONE```. That means if
the address is removed the person won't be removed.

```cpp
  struct address
  {
    std::string street;
    std::string city;
    matador::object_ptr<person> citizen;

    template < typename Operator >
    void process(Operator &op)
    {
      // ...
      matador::access::belongs_to(op, "citizen", citizen, cascade_type::NONE);
    }
  };
```

In the persons declaration we add an ```matador::object_ptr<address>``` and
the call to ```has_one()``` to the class. Here we use ```matador::cascade_type::ALL```.
That means if the person is removed the address is removed as well.
That's it. Now we have a one to one relationship beetween two classes.

```cpp
  struct person
  {
    // ...
    matador::object_ptr<address> addr;

    template < typename Operator >
    void process(Operator &op)
    {
      // ...
      matador::access::has_one(op, "address", addr, cascade_type::ALL);
    }
  };
```

**Note:** With this kind of relationship we have a hard linked
relationship. Which means if we remove the person from
our store the address object is removed as well. The
```matador::cascade_type::ALL``` means the all operation of
```INSERT```, ```UPDATE``` and ```DELETE``` will take
affect on the member address as well
{: .bs-callout .bs-callout-warning}

When using this construct the matador will take care of the following:

- If a person object is set into address' ```citizen``` field the persons
```address``` field is automatically updated with this address.
- If an address object is set into persons ```address``` field the address'
```citizen``` field is automatically updated with this person.

```cpp
  // setup session/object_store
  auto george = s.insert<person>("george");
  auto home = s.insert<address>("homestreet", "homecity");

  george.modify()->addr = home;

  // person george will be set into address
  std::cout << "citizen: " << home->citizen->name << "\n";
```

### OneToMany Relations

When it comes to one to many relationships it is also
quiet easy. matador comes with a handy wrapper to the STL container
class ```std::vector``` and ```std::list```.

**Note:** The STL container class ```std::set``` and ```std::unordered_set```
will be supported soon.
{: .bs-callout .bs-callout-warning}

Because these classes are designed in the same way as
the STL classes we can use them in the same way plus the
benefit of the relationship.

We change our handy ```person``` class that it has a lot of addresses.
The ```address``` class can stay untouched because the ```belongs_to``` part
doesn't need to change.

```cpp
  struct person
  {
    std::string name;
    matador::has_many<address> addresses;

    template < typename Operator >
    void process(Operator &op)
    {
      // ...
      matador::access::has_many(op, "address", addresses, "person_id", "address_id");
    }
  };
```
Now we can add several addresses to a person object and address' ```citizen```
field is filled again automatically.
But it works also in the opposite way: If a person is set into an address the
address is automatically added to persons address list.

```cpp
  // ...
  // create a new person
  auto joe = s.insert<person>("joe");
  auto home = s.insert<address>("homestreet", "homecity");
  auto work = s.insert<address>("workstreet", "workcity");

  joe.modify()->addresses.push_back(home);
  // homes citicen will be joe
  std::cout << "citizen: " << home->citizen->name << "\n";

  work.modify()->citizen = joe;
  // joes addresses have now increased to two
  std::cout << "joes addresses: " << joe->addresses.size() << "\n";
  ```

  Now we can simply iterate over the list like we used to
  do it with all STL containers..

  ```cpp
  // access all friends
  for (const auto &addr : joe->addresses) {
    std::cout << "address street: " << addr->street << "\n";
  }
```

### ManyToMany Relations

Many to many relationships can also be used straight forward. Asume we
have a class ```student``` taking a list of courses and a class ```course```
having a list of students.

```cpp
  struct student
  {
    matador::has_many<course> courses;

    template < typename Operator >
    void process(Operator &op)
    {
      // ...
      matador::access::has_many(op, "student_course", courses, "student_id", "course_id");
    }
  };

  struct course
  {
    matador::has_many<student> students;

    template < typename Operator >
    void process(Operator &op)
    {
      // ...
      matador::access::has_many(op, "student_course", students, "student_id", "course_id");
    }
  };
```
Once a student adds a course to its course list the student is added to the list
of students of the course. And the other way around if a student is added to a course
the course is added to the list of students course list.

```cpp
  auto jane = s.insert<student>("jane");
  auto art = s.insert<course>("art");
  auto algebra = s.insert<course>("algebra");

  jane.modify()->courses.push_back(art);
  std::cout << art->students.front()->name << "\n"; // prints out 'jane'

  art.modify()->students.push_back(jane);
  std::cout << jane->courses.size() << "\n"; // prints out '2'
```
