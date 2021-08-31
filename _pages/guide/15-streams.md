## Streams

Matador provides a stream module which allows the user to
process a range in a streaming way applying element processor
(like map, filter etc.) and terminators (collecting processed elements,
evaluate expressions on resulting elements).

{% highlight cpp linenos %}
auto result = make_stream({1, 2, 3, 4, 9, 4, 2, 6})  // create a range of numbers
    .skip(3)                                         // skips 1, 2 and 3
    .take(2)                                         // takes 4 and 9
    .map([](const auto &val){ return val * 2; })     // multiplies each value with 2
    .filter([](const auto &val){ return val > 10; }) // filters a values greater 10
    .collect<std::vector>();                         // collects the resulting
                                                     // elements into a vector
{% endhighlight %}

### Generators

Generator functions helps creating streams from containers and initializer lists
or defined ranges with start and end and optionally a step size.

Furthermore an endless counter stream generator exists.

#### Make stream

With the ```make_stream(...)``` generator function it is possible to create several 
kinds of streams.

To create a stream from an existing container, just pass the container to the
function ```make_stream(container)```.

{% highlight cpp linenos %}
std::vector<std::string> string_vec { "one", "two", "three", "four" };

auto result = make_stream(string_vec);
{% endhighlight %}


The ```make_stream(initializer_list)``` can also directly take an initializing
list of elements to create a stream.

{% highlight cpp linenos %}
auto result = make_stream({ "one", "two", "three", "four" });
{% endhighlight %}


It is also possible to create ranges with ```make_stream(from, to)```. Where _from_ and
_to_ are included values in the generated stream.

{% highlight cpp linenos %}
// creates a stream of ints from 1 to 8
auto result = make_stream(1, 8);

// resulting stream: 1, 2, 3, 4, 5, 6, 7, 8
{% endhighlight %}

In an extended version of the ```make_stream(from, to, stepsize)``` generator function a
stepsize value can be added indicating the incremental value for the generated stream.

{% highlight cpp linenos %}
// creates a stream of ints from 1 to 8 with a step of two
auto result = make_stream(1, 8, 2);

// resulting stream: 1, 3, 5, 7
{% endhighlight %}

#### Make stream counter

The ```make_stream_counter(start)``` generator creates an endless steam starting from _start_.

{% highlight cpp linenos %}
auto result = make_stream_counter(1)
    .take(5)
    .collect<std::vector>();

// result: 1, 2, 3, 4, 5
{% endhighlight %}

In an extended version of the ```make_stream(start, stepsize)``` generator the stepsize
parameter defines the increment value of the endless stream.

{% highlight cpp linenos %}
auto result = make_stream_counter(1, 2)
    .take(5)
    .collect<std::vector>();
// result: 1,3,5,7,9
{% endhighlight %}

### Element processors

#### Take

The ```take(count)``` processor takes the first _count_ elements of
the incoming range and passes them to the next processor. All further
elements are ignored and the stream is completed.

{% highlight cpp linenos %}
auto result = make_stream_counter(6)
    .take(3)
    .collect<std::vector>();
{% endhighlight %}

#### Take while

The ```take_while(predicate)``` processor takes all elements matching
the given predicate of the incoming range and passes them to the next
processor.

{% highlight cpp linenos %}
auto result = make_stream_counter(6)
    .take_while([](const int &i) { return i > 1 && i < 4; })
    .collect<std::vector>();
{% endhighlight %}

#### Skip

The ```skip(count)``` processor skips the first _count_ elements of
the incoming range and passes following elements to the next processor.

{% highlight cpp linenos %}
auto result = make_stream_counter(6)
    .skip(5)
    .collect<std::vector>();
{% endhighlight %}

#### Skip while

The ```skip_while(predicate)``` processor skips all elements matching
the given predicate of the incoming range. All elements __not__ matching
the predicate are passed to the next processor.

{% highlight cpp linenos %}
auto result = make_stream_counter(6)
    .skip_while([](const int &i) { return i > 1 && i < 4; })
    .collect<std::vector>();
{% endhighlight %}

#### Every

The ```every(count)``` processor passes every _count_ element of the
incoming elements to the next processor.

{% highlight cpp linenos %}
auto result = make_stream(1, 12)
    .every(3)
    .collect<std::vector>();
{% endhighlight %}

#### Filter

The ```filter(predicate)``` processor applies the predicate to each
incoming element. If the predicate returns ```true``` the element is
passed to the next processor otherwise the element is skipped.

{% highlight cpp linenos %}
bool is_even(int val) { return val % 2 == 0; }

auto result = make_stream(1, 8)
    .filter(is_even)
    .collect<std::vector>();
{% endhighlight %}

#### Map

The ```map(predicate)``` processor applies the predicate to each
incoming element. The predicate modifies the element or transforms
it to new element of a different type and returns it. The returned
element is than passed to the next processor.

{% highlight cpp linenos %}
auto result = make_stream(1, 8)
    .map([](const int &i) { return std::to_string(i); })
    .collect<std::vector>();
{% endhighlight %}

#### Flat map

The ```flatmap(predicate)``` flattens a nested container of the incoming elements
(list, vectors, i.e.) into one new stream of elements containing
all elements of the container.

{% highlight cpp linenos %}
auto result = make_stream<person>({
    {"otto",   34, { "red", "green" }},
    {"jane",   27, { "blue"}},
    {"george", 56, { "brown", "purple" }},
    {"bobby",  15, { "yellow", "gold", "silver" }}
  }).flatmap([](const person &p) {
    return p.colors;
  }).collect<std::vector>();
{% endhighlight %}

#### Peek

The ```peek(predicate)``` processor provides access to each incoming element
in the stream and passes it to the next processor. It can be used to check
or debug each stream element.

{% highlight cpp linenos %}
int result = 0;
make_stream(3, 9)
    .take(1)
    .peek([&result](const int &val) { result = val; })
    .count();
{% endhighlight %}

#### Concat

The ```concat(stream)``` concatenates the current stream with the
given stream of the same type. Once the current stream finishes the
elements of the next stream are processed seamlessly.

{% highlight cpp linenos %}
auto s = make_stream(6, 10);
auto result = make_stream(1, 5)
    .concat(s)
    .collect<std::vector>();
{% endhighlight %}

#### Pack every

The ```pack_every(packsize)``` processor packs every _n_ elements of the stream into
a container (vector) and create a new stream of it.

{% highlight cpp linenos %}
auto result = make_stream(1, 13)
    .pack_every(3)
    .collect<std::vector>();
{% endhighlight %}

The exmaple above leads to a collection/stream of the following elements:

{% highlight cpp linenos %}
std::vector<std::vector<int>> result = { {1,2,3},{4,5,6},{7,8,9},{10,11,12},{13} };
{% endhighlight %}

### Terminators

#### First

The ```first()``` terminator returns an optional with the first element of the
resulting element list. If no first element is in the list a _null_ optional
is returned.

{% highlight cpp linenos %}
auto first_value = make_stream(1, 8)
    .first();
{% endhighlight %}

#### Last

The ```last()``` terminator returns an optional with the last element of the
resulting element list. If no last element is in the list a _null_ optional
is returned.

{% highlight cpp linenos %}
auto first_value = make_stream(1, 8)
    .last();
{% endhighlight %}

#### Min

The ```min()``` terminator determines the minimum value of all elements in the stream.

{% highlight cpp linenos %}
auto minval = make_stream(1, 8)
    .min();
{% endhighlight %}

#### Max

The ```max()``` terminator determines the maximum value of all elements in the stream.

{% highlight cpp linenos %}
auto minval = make_stream(1, 8)
    .max();
{% endhighlight %}

#### At

The ```at(index)``` terminator access an element at the requested index and return a
```optional<T>``` containing the element. If the index isn't valid a null optional is
returned.

{% highlight cpp linenos %}
auto value_at_index = make_stream(1, 8)
    .at(4);
{% endhighlight %}

#### Any

The ```any(predicate)``` terminator returns ```true``` if any of the incoming elements
applies to the given predicate.

{% highlight cpp linenos %}
bool is_even(int val) { return val % 2 == 0; }

bool is_any_even = make_stream(1, 6).any(is_even);
{% endhighlight %}

#### All

The ```all(predicate)``` terminator returns ```true``` if all of the incoming elements
applies to the given predicate.

{% highlight cpp linenos %}
bool is_less_seven(int val) { return val < 7; }

bool are_all_less_seven = make_stream(1, 6).all(is_less_seven);
{% endhighlight %}

#### None

The ```none(predicate)``` terminator returns ```true``` if none of the incoming elements
applies to the given predicate.

{% highlight cpp linenos %}
bool is_greater_seven(int val) { return val > 7; }

bool are_all_greater_seven = make_stream(1, 6).none(is_greater_seven);
{% endhighlight %}

#### Count

The ```count()``` terminator returns the current number of elements in the stream.

{% highlight cpp linenos %}
auto size = make_stream(1, 8)
    .count();
{% endhighlight %}

A second version of ```count(predicate)``` takes a predicate and counts all elements
for which the predicate returns ```true```.

{% highlight cpp linenos %}
bool is_even(int val) { return val % 2 == 0; }

auto size = make_stream(1, 8)
    .count(is_even);
{% endhighlight %}

#### Reduce

The ```reduce(accumulator)``` terminator are reduced to one value applying the given
accumulator function. The function takes the current accumulated value and the current
element and returns the new accumulated value. An ```optional<T>``` is returned, because
the stream could be empty and then a null optional is returned.

{% highlight cpp linenos %}
auto sum = make_stream(1, 8)
    .reduce(std::plus<>());
{% endhighlight %}

To always return a valid value it a second version of ```reduce(initial, accumulator)```
exists taking an initial value and the accumulator function. For the first element in the
stream the initial value is used than it it works like the first version of this function.
If the stream is empty the inital value is returned.

{% highlight cpp linenos %}
auto reduce_identity_result = make_stream(1, 8).reduce(std::string(), [](const std::string &result, int i) {
    std::stringstream istr;
    if (!result.empty()) {
      istr << ",";
    }
    istr << i;
    return result + istr.str();
  });
{% endhighlight %}

The third ```reduce_idfunc(initial_func, accumulator)``` terminator takes a function creating the
initial value for the first stream element. 

{% highlight cpp linenos %}
auto reduce_identity_func_result = make_stream(1, 8).reduce_idfunc([](const int &val) {
    std::stringstream istr;
    istr << val;
    return istr.str();
  }, [](const std::string &result, int i) {
    std::stringstream istr;
    istr << " <-> " << i;
    return result + istr.str();
  });
{% endhighlight %}

#### Collect

The ```collect<C>()``` terminator collects all resulting elements into a
new container. The type of the container must be provided within the
template type.

{% highlight cpp linenos %}
auto result = make_stream(3, 17)
    .collect<std::vector>();
{% endhighlight %}

#### For each

The ```for_each(predicate)``` terminator applies the given predicate function to
each resulting element of the stream.

{% highlight cpp linenos %}
auto result = make_stream(3, 17)
    .for_each([](const auto &val){
        std::cout << "element: " << val << "\n";
    });
{% endhighlight %}

#### Join

The ```join()``` terminator concats all elements of a stream to a string and comes in
three flavours. The first one takes no arguments and just concats all elements. The second
```joind(delimiter)``` terminator takes a delimiter and inserts that delimiter between
every stream element when concatenating.
The third and last ```join(delimiter, suffix, prefix)``` terminator takes additionally
to the delimiter string a suffix and prefix string which will be prepended and appended
to the resulting string.

{% highlight cpp linenos %}
auto result = make_stream({"one", "two", "three", "four"}).join();

// result: "onetwothreefour"

result = make_stream({"one", "two", "three", "four"}).join(", ");

// result: "one, two, three, four"

result = make_stream({"one", "two", "three", "four"}).join(", ", "[", "]");

// result: "[one, two, three, four]"
{% endhighlight %}
