---
title: "Objects"
---

### Objects

All kind of object up from POD can be attached to the object store. It can be a
simple struct with publiv members or a complex class hierarchy with bases classes and virtual
methods.

The only thing that must exists is a `serialize` function.

```cpp
struct person
{
    std::string name;
    unsigned age;
    
    template < class SERIALIZER >
    void serialize(SERIALIZER &serializer)
    {
        serializer.serialize("name", name);
        serializer.serialize("age", age);
    }
};
```

Provide such a method for all objects to be attached to the object store and you're done.
