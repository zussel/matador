## Relations

When it comes to object relations you can use one to one, one to many and
many to many relations in a straight forward way. Therefor object store provides
easy wrapper for these relations

### OneToOne Relations

In this example we have two object types an address class
and a person class. The address class acts as the ```belongs_to``` part
of the relation. So we add a ```matador::belongs_to<person>``` to our address
class. Don't forget to add the appropriate ```serialize``` to the
serialize method.

For the person in address we add the ```cascade_type::NONE```. That means if
the address is removed the person won't be removed.

{% highlight cpp linenos %}
struct address
{
  std::string street;
  std::string city;
  matador::belongs_to<person> citizen;

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    // ...
    serialize("citizen", citizen, cascade_type::NONE);
  }
};
{% endhighlight %}

In the persons declaration we add an ```matador::has_one<address>``` and
the call to ```serialize``` to the class. Here we use ```cascade_type::ALL```.
That means if the person is removed the address is removed as well.
That's it. Now we have a one to one relationship beetween two classes.

{% highlight cpp linenos %}
struct person
{
  // ...
  matador::hans_one<address> addr;

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    // ...
    serialize("address", addr, cascade_type::ALL);
  }
};
{% endhighlight %}

**Note:** With this kind of relationship we have a hard linked
relationship. Which means if we remove the person from
our store the address object is removed as well. The
```cascade_type::ALL``` means the all operation of
```INSERT```, ```UPDATE``` and ```DELETE``` will take
affect on the member address as well
{: .bs-callout .bs-callout-warning}

When using this construct the matador will take care of the following:

- If a person object is set into address' ```citizen``` field the persons
```address``` field is automatically updated with this address.
- If an address object is set into persons ```address``` field the address'
```citizen``` field is automatically updated with this person.

{% highlight cpp linenos %}
// setup session/object_store
auto george = s.insert(new person("george"));
auto home = s.insert(new address("homestreet", "homecity"));

george.modify()->addr = home;

// person george will be set into address
std::cout << "citizen: " << home->citizen->name << "\n";
{% endhighlight %}

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

{% highlight cpp linenos %}
struct person
{
  std::string name;
  matador::has_many<address> addresses;

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    // ...
    serialize("address", addresses, "person_id", "address_id");
  }
};
{% endhighlight %}
Now we can add several addresses to a person object and address' ```citizen```
field is filled again automatically.
But it works also in the opposite way: If a person is set into an address the
address is automatically added to persons address list.

{% highlight cpp linenos %}
// ...
// create a new person
auto joe = s.insert(new person("joe"));
auto home = s.insert(new address("homestreet", "homecity"));
auto work = s.insert(new address("workstreet", "workcity"));

joe.modify()->addresses.push_back(home);
// homes citicen will be joe
std::cout << "citizen: " << home->citizen->name << "\n";

work.modify()->citizen = joe;
// joes addresses have now increased to two
std::cout << "joes addresses: " << joe->addresses.size() << "\n";
{% endhighlight %}

Now we can simply iterate over the list like we used to
do it with all STL containers..

{% highlight cpp linenos %}
// access all friends
for (const auto &addr : joe->addresses) {
  std::cout << "address street: " << addr->street << "\n";
}
{% endhighlight %}

### ManyToMany Relations

Many to many relationships can also be used straight forward. Asume we
have a class ```student``` taking a list of courses and a class ```course```
having a list of students.

{% highlight cpp linenos %}
struct student
{
  matador::has_many<course> courses;

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    // ...
    serialize("student_course", courses, "student_id", "course_id");
  }
};

struct course
{
  matador::has_many<student> students;

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    // ...
    serialize("student_course", students, "student_id", "course_id");
  }
};
{% endhighlight %}
Once a student adds a course to its course list the student is added to the list
of students of the course. And the other way around if a student is added to a course
the course is added to the list of students course list.

{% highlight cpp linenos %}
auto jane = s.insert(new student("jane"));
auto art = s.insert(new course("art"));
auto algebra = s.insert(new course("algebra"));

jane.modify()->courses.push_back(art);
std::cout << art->students.front()->name << "\n"; // prints out 'jane'

art.modify()->students.push_back(jane);
std::cout << jane->courses.size() << "\n"; // prints out '2'
{% endhighlight %}
