#include "object/serializable.hpp"
#include "serializer.hpp"
#include "object/object_store.hpp"
#include "object/object_ptr.hpp"
#include "object/object_view.hpp"

#include "database/session.hpp"
#include "database/transaction.hpp"

#include <string>
#include <iostream>
#include <exception>

/*
 * the person class
 */
class person : public oos::serializable
{
public:
  // primary key
  oos::identifier<long> id;
  // properties
  std::string name;
  int age;

public:
  person() {}
  person(const std::string &n)
    : name(n)
  {}
  virtual ~person() {}
  
  virtual void serialize(oos::object_writer &wrt) const
  {
    wrt.write("id", id);
    wrt.write("name", name);
    wrt.write("age", age);
  }

  virtual void deserialize(oos::object_reader &rdr)
  {
    rdr.read("id", id);
    rdr.read("name", name);
    rdr.read("age", age);
  }
};

int main(int, char *[])
{
  /*
   * create an object store
   * and insert a prototype for
   * our person class
   */
  oos::object_store ostore;

  ostore.insert_prototype<person>("person");
  
  /*
   * create a sql session
   * with a connection string
   * to a sql (in this case
   * to a sqlite sql)
   * 
   * then call create to setup the
   * session, object store and tables.
   */
  oos::session db(ostore, "sqlite://person.db");

  db.create();

  /*
   * insert a new object directly
   * using the session
   */
  typedef oos::object_ptr<person> person_ptr;

  person_ptr p = db.insert(new person("Theo"));

  /*
   * insert several objects using
   * a transaction.
   * 
   * after starting the transaction
   * use the object store to insert
   * objects
   */
  oos::transaction tr(db);

  // start transaction
  try {  
    tr.begin();

    ostore.insert(new person("George"));
    ostore.insert(new person("Jane"));
    ostore.insert(new person("Tim"));
    ostore.insert(new person("Walter"));

    tr.commit();
  } catch (std::exception&) {
    // an error occurred: do rollback
    tr.rollback();
  }
  
  /*
   * to access the objects one
   * have to open a view over
   * the requested object type
   * 
   * then one can iterate over the
   * view and access or modify the
   * objects
   */
  typedef oos::object_ptr<person> person_ptr;
  typedef oos::object_view<person> person_view_t;

  person_view_t oview(ostore);

  person_view_t::iterator i = oview.begin();
  person_view_t::iterator last = oview.end();
  for (; i != last; ++i) {
    person_ptr p = *i;
    std::cout << "person: " << p->name() << "\n";
  }

  return 0;
}

