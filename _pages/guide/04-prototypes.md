## Prototypes

Once all entities are designed you want to store them in the object store. Before this could be done
all entities must be introduced into the object store.

Assume we have an abstract base class ```Vehicle``` and derived from this the
classes ```Truck```, ```Car``` and ```Bike```. Now lets make this hierarchy known to the
matador::object_store:

{% highlight cpp linenos %}
matador::object_store ostore;
ostore.attach<Vehicle>("vehicle", true);
ostore.attach<Truck, Vehicle>("truck");
ostore.attach<Car, Vehicle>("car");
ostore.attach<Bike, Vehicle>("bike");
{% endhighlight %}

As you can see it is quite simple to add the hierarchy to
the matador::object_store by calling method ```matador::object_store::attach```.
As there are several ways to call this method we decide to use the one
with template arguments.

In the example above the vehicle class is an abstract base. Here we need only one template
argument (the class itself: Vehicle). With the first method parameter you give your
type a unique name. The second parameter is a flag telling the ```matador::object_store```
that this type is abstract. Settings this flag to true you can't insert objects of this
abstract type.

### Inserting Objects

Now that we've setup up our hierarchy we can insert objects into
the matador::object_store.

{% highlight cpp linenos %}
typedef object_ptr<vehicle> vehicle_ptr;

vehicle_ptr truck = ostore.insert(new Truck("MAN");
vehicle_ptr car   = ostore.insert(new Car("VW Beetle");
vehicle_ptr bike  = ostore.insert(new Bike("Honda");
{% endhighlight %}
As you can see we use matador::object_ptr of type vehicle. The vehicle class in
our example is the abstract base class for all concrete vehicle types.
So the concrete vehicle object is inserted correctly and assigned to the
object_ptr.

That means once you have inserted an object of any concrete type you access
it via an appropriate object_ptr afterwards. You should never work with the
raw instance pointer. This could lead to inconsistencies.

### Updating Objects

Now that we have some objects inserted we may want
to modify them. The important thing here is as mentioned above
that you don't deal with raw pointer to your object when try
to modify it. You always have a pointer object wrapped
around the object (like shared_ptr).
The ```matador::object_store``` returns an ```matador::object_ptr``` when an
object is inserted. Once you received the ```matador::object_ptr```
you can change your object by using it like usual pointer.

{% highlight cpp linenos %}
typedef matador::object_ptr<Truck> truck_ptr;
truck_ptr truck = ostore.insert(new Truck("MAN"));

truck->weight = 2.5;
truck->color = red;
{% endhighlight %}
**Note:** When using the raw pointer object store can't guarantee that a change is
properly rolled back when an exception occurs.
{: .bs-callout .bs-callout-warning}

### Deleting Objects

An already inserted object can be deleted from the object store using the ```remove```
method:

{% highlight cpp linenos %}
typedef matador::object_ptr<Truck> truck_ptr;
truck_ptr truck = ostore.insert(new Truck("MAN"));

if (ostore.is_removable(truck)) {
  // remove object
  ostore.remove(truck);
} else {
  // object can't be removed
}
{% endhighlight %}
The object store will check if the object is removable (i.e. there are no other objects
referencing this object). If the object is not removable an ```object_exception``` is
thrown.
One can do this check manually by calling simply ```object_store::is_removable``` (as shown
in the example above).
