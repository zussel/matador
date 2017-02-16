has_many<employee> <-> belongs_to<department>
===================================================

```
struct employee
{
  oos::belongs_to<department> dep;

  template < class S >
  void serialize(S &s)
  {
    s.serialize("department", dep);
  }
};

struct department
{
  oos::has_many<employee> emps;

  template < class S >
  void serialize(S &s)
  {
    s.serialize("employee", emps);
  }
};

auto george    = ostore.insert(new employee("george"));
auto insurance = ostore.insert(new department("insurance"));

insurance->emps.push_back(george);

```

Analyze phase
=============

__Case 1: Department attached first__

 * attach department
 * prototype "employee" not found; prepare new
 * mark as has many relation for "department" member "employees"
 * attach employee
 * prototype "employee found"
 * if employee contains belongs_to for "department" tie prototypes
 * else throw error

__Case 2: Employee attached first__

 * attach employee
 * prototype "department" not found: prepare new
 * mark as belongs relation for "employee" member "department"
 * attach department
 * prototype "department" found
 * if department contains has_many for "employee" tie prototypes
 * else throw error

Business phase
==============

__Case 1: emplyoee sets a department__

 * employee gets object type "department" with member name "employees" from prototype
 * employee is added to departments employees container

__Case 2: department adds an employee__

 * department gets object type "employee" with member name "department" from prototype
 * department is set for employee


TODO
====

1. Add ```object_store::attach<T>(prototype_node, parent, [observer])```
2. Add static
 * ```prototype_node::make_node<T>()```
 * ```prototype_node::make_relation_node<T>()```
3. Adjust ```node_anaylzer```
