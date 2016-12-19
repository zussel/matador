#include "object/object.hpp"
#include "object/object_atomizer.hpp"
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
class person : public oos::object
{
private:
  std::string name_;
  int age_;

public:
  person() {}
  person(const std::string &n)
    : name_(n)
  {}
  virtual ~person() {}
  
  virtual void serialize(oos::object_writer &wrt) const
  {
    oos::object::serialize(wrt);
    wrt.write("name", name_);
  }

  virtual void deserialize(oos::object_reader &rdr)
  {
    oos::object::deserialize(rdr);
    rdr.read("name", name_);
  }

  void name(const std::string &n)
  {
    modify(name_, n);
  }

  std::string name() const
  {
    return name_;
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
   * create a database session
   * with a connection string
   * to a database (in this case
   * to a sqlite database)
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

