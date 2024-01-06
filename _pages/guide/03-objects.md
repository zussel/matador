## Objects

All kind of object up from POD can be attached to the object store. It can be a
simple struct with publiv members or a complex class hierarchy with bases classes and virtual
methods.

The only thing that must exist is a `process` method. This method takes as parameter
an operator.

```cpp
struct person
{
  unsigned long id{};
  std::string name;
  matador::date birthday;

  template < typename Operator >
  void process(Operator &op)
  {
    namespace field = matador::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, 255);
    field::attribute(op, "birthday", birthday);
  }
};
```

Provide such a method for all objects to be attached to the object store and you're done.

Within the namespace ```matador::access``` one can specify what type of column
is added to the entity. The following types are available (called via function):

### Primary Key

Primary key is added via ```primary_key``` function. With this function the
added column will have the constraint ```PRIMARY KEY```. Currently two types
of primary keys are supported: Integral type and varchar e.g. string with size
(leading to db type ```VARCHAR```)

```cpp
// add primary key where field member is an integral type
matador::access::primary_key(op, "id", id);
// add primary key where field member is of type string and
// size is the size of the string. This leads to a VARCHAR(size) database column type.
matador::access::primary_key(op, "name", name, 255);
```

The process method for integral types looks like this:

```cpp
struct identifier_type_integral
{
  unsigned long id{};

  template < typename Operator >
  void process(Operator &op)
  {
    matador::access::primary_key(op, "id", id);
  }
};
```

Below an example of a string primary key. Note that the size parameter is mandatory.
```cpp
struct identifier_type_string
{
  std::string id{};

  template < typename Operator >
  void process(Operator &op)
  {
    matador::access::primary_key(op, "id", id, 255);
  }
};
```

### Attributes

The ```attribute(...)``` function takes the following arguments:

```attribute(<Operator>, <field name>, <field member>, <field attributes>)```

While the first three parameters are the usual suspects operator, field name and
field member, the fourth is of type ```field_attributes``` and consists of a size
value and field constraints.
The size is only relevant for type ```std::string``` and ```const char*``` and leads
when greater than zero to a db field type of ```VARCHAR```.
The field constraints handle the well known constraints

- ```NOT NULL```
- ```UNIQUE```
- ```PRIMARY KEY```
- ```FOREIGN KEY```

**Note:** In a future release field attributes will also support ```INDEX``` and ```DEFAULT```.
{: .bs-callout .bs-callout-warning}

To allow an easy instantiation within the call to ```matador::access::attribute``` the class has a couple
of constructors allowing to write the following:

```cpp
// string column with size of 255: VARCHAR(255)
matador::access::attribute(op, "name", name, 255);
// column with NOT NULL constraint
matador::access::attribute(op, "value", value, constraints::NOT_NULL);
// string column with size of 255 and unique constraint:
matador::access::attribute(op, "name", name, {255, constraints::UNIQUE});
```

### Relations

To add a foreign key relationship matador provides the functions ```has_one()```, ```belongs_to()``` and ```has_many()```.
Object relations are supported by using ```object_ptr<Type>```.

The syntax of ```has_one()``` and ```belongs_to()``` looks as follows:

```cpp
matador::has_one(<operator>, <field name>, <field member>, <cascade>);
matador::belongs_to(<operator>, <field name>, <field member>, <cascade>);
```

The first parameter is the operator in charge. The second parameter
is the name of the foreign table. The third is the relation object member.
With the fourth parameter in the process method adjusts the cascading
command behavior when an insert, update, delete and
select operation is executed on the containing object.
```matador::cascade_type::ALL``` means that all operations
are also passed to the foreign object (e.g. when containing object is
deleted the foreign is deleted as well).

And the syntax of ```has_many()``` looks this way. There are
two variants of the ```has_many()``` function:


```cpp
// This function creates the name of the relation table columns on its own
matador::access::has_many(<Operator>, <relation table name>, <container member>, <cascade>);
// This function takes the given names as the relation table column names
matador::access::has_many(<Operator>, <relation table name>, <container member>, <left column name>, <right column name>, <cascade>);
```
The first parameter is the operator in charge. The second parameter
is the name of the foreign table. The third is the relation object member.
With the fourth parameter in the process method adjusts the cascading
command behavior when an insert, update, delete and
select operation is executed on the collection. ```matador::cascade_type::ALL``` means
that all operations are also passed to the collection.
The optional fifth and sixth parameter defining the names of the relation
table columns. If not set matador decide the names.

#### Has One Relation

```has_one()``` acts like a simple foreign key relation or as the counterpart for a belongs to relation. It creates
a column for a foreign key constraint. Therefor the entity struct must contain a member of type ```matador::object_ptr<ForeignType>```.

```cpp
struct foreign_key
{
  matador::object_ptr<T> has_one_;         // has one relationship

  template < typename Operator >
  void process(Operator &op)
  {
    namespace field = matador::access;
    field::has_one(op, "has_one_table", has_one_, cascade_type::ALL);
  }
};
```

#### Belongs To Relation

```belongs_to()``` acts like the name indicates as a belongs to relation ship. It needs as a 
counterpart a ```has_one``` or a ```has_many``` relationship (described below). It creates
a column for a foreign key constraint. Therefor the entity struct must contain a member of
type ```matador::object_ptr<ForeignType>```.

```cpp
struct foreign_key
{
  matador::object_ptr<T> belongs_to_;      // belongs to relationship

  template < typename Operator >
  void process(Operator &op)
  {
    namespace field = matador::access;
    field::belongs_to(op, "belongs_to_table", belongs_to_, cascade_type::ALL);
  }
};
```

#### Has Many Relation

```has_many()``` acts as the _many_ side of a ```belongs_to()``` or another ```has_many()```
relation. If the other side is the latter one a relation table is created automatically.
Collections are supported by ```container<Type, ContainerType>```. By now ```container``` supports
internal two container types ```std::vector``` (default) and ```std::list```.

```cpp
struct relations
{
  matador::container<T> collection_; // a collection with objects

  template < typename Operator >
  void process(Operator &op)
  {
    namespace field = matador::access;
    field::has_many(op, "collection", collection_, "local_id", "foreign_id", cascade_type::NONE);
  }
};
```

```container<Type>``` describes a collection leading to a relation table/object between
the containing object and the foreign object. If the foreign object is of
a simple builtin type (e.g. ```int``` or ```matador::date```) the value
itself is stored in the relation table.
