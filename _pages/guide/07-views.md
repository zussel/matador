## Views

In most cases we want to iterate over all objects of a certain
type. How can we achieve this? We open a view for the type we want
to inspect.

Again we use our little person class and insert some persons into
our store.

{% highlight cpp linenos %}
  // add some friends
  ostore.insert<person>("joe")
  ostore.insert<person>("walter");
  ostore.insert<person>("helen");
  ostore.insert<person>("tim");
{% endhighlight %}

Than we create a view with the ```matador::object_view``` class. This class
takes as the template parameter the desired class type. Then we can
use the view like a STL list containing ```matador::object_ptr``` of our
desired type.

{% highlight cpp linenos %}
  // shortcut to the person view
  using person_view_t = matador::object_view<person>;

  person_view_t pview(ostore);

  person_view_t::iterator i = pview.begin();
  for (const auto &p : pview) {
    std::cout << "person: " << p->name << std::endl;
  }
{% endhighlight %}

But this class can to somethig more for us. If we have a hierarchy
of classes, we can create a view of the base type and easily iterate
overall sub-types.

Look at the next example. We have a hierarchy with the person class
as base class and inherited from that we have classes student and
employee.
And again we create a view of type person to access all objects of
type person including sub-types student and employee.

{% highlight cpp linenos %}
  class student : public person { //... };
  class employee : public person { //... };

  matador::object_store ostore;
  ostore.attach<person>("person");
  ostore.attach<student, person>("student");
  ostore.attach<employee, person>("employee");

  using person_view_t = matador::object_view<person>;

  person_view_t pview(ostore);

  person_view_t::iterator i = pview.begin();
  for (const auto &p : pview) {
    std::cout << p->name() << std::endl;
  }
{% endhighlight %}
