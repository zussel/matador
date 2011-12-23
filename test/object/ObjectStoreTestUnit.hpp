#ifndef OBJECTSTORE_TESTUNIT_HPP
#define OBJECTSTORE_TESTUNIT_HPP

#include "unit/unit_test.hpp"

#include "object/object.hpp"
#include "object/object_ptr.hpp"
#include "object/object_store.hpp"
#include "object/object_atomizer.hpp"

class ObjectStoreTestUnit : public oos::unit_test
{
private:
  class SimpleObject : public oos::object
  {
  public:
    SimpleObject()
      : number_(0)
    {}
    SimpleObject(const std::string &n)
      : number_(0)
      , name_(n)
    {}
    SimpleObject(int n)
      : number_(n)
    {}
    SimpleObject(const std::string &n, int i)
      : number_(i)
      , name_(n)
    {}
    virtual ~SimpleObject() {}
    
    void read_from(oos::object_atomizer *reader)
    {
      object::read_from(reader);
      reader->read_string("name", name_);
      reader->read_int("number", number_);
    }
    void write_to(oos::object_atomizer *writer) const
    {
      object::write_to(writer);
      writer->write_string("name", name_);
      writer->write_int("number", number_);
    }

    int number() const { return number_; }
    void number(int n) { number_ = n; }

    std::string name() const { return name_; }
    void name(const std::string &n) { name_ = n; }

  private:
    int number_;
    std::string name_;
  };
  class ObjectWithSubObject : public oos::object
  {
  public:
    ObjectWithSubObject() {}
    virtual ~ObjectWithSubObject() {}

    void read_from(oos::object_atomizer *reader)
    {
      object::read_from(reader);
      reader->read_object("simple", simple_);
    }
    void write_to(oos::object_atomizer *writer) const
    {
      object::write_to(writer);
      writer->write_object("simple", simple_);
    }

    oos::object_ptr<SimpleObject> simple() const { return simple_; }
    void simple(const oos::object_ptr<SimpleObject> &s) { simple_ = s; }

  private:
    oos::object_ptr<SimpleObject> simple_;
  };

public:
  ObjectStoreTestUnit();
  virtual ~ObjectStoreTestUnit();
  
  virtual void initialize();
  virtual void finalize();
  
  void access_value();
  void simple_object();
  void object_with_sub_object();
  void multiple_simple_objects();
  void multiple_object_with_sub_objects();
  void sub_delete();

private:
  oos::object_store ostore_;
};

#endif /* OBJECTSTORE_TESTUNIT_HPP */
