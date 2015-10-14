#ifndef TABLE_READER_HPP
#define TABLE_READER_HPP

#include "object/object_preparator.hpp"
#include "object/object_atomizer.hpp"

namespace oos {

class object_store;
class object_proxy;

class database;
template < class T > class result;
class table;

class table_reader : public generic_object_reader<table_reader>
{
public:
  table_reader(table &t, object_store &ostore);
  virtual ~table_reader() {}

  void load(result<serializable> &res);

  template < class T >
  void read_value(const char *, T &) {}
  void read_value(const char *, char *, int ) {}
  void read_value(const char *, object_base_ptr &x);
  void read_value(const char *id, object_container &x);

private:
  // temp data while loading
  object_proxy *new_proxy_;
  object_store &ostore_;
  object_preparator object_preparator_;
  table &table_;
};

}
#endif /* TABLE_READER_HPP */
