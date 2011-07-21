#ifndef OBJECT_PROXY_HPP
#define OBJECT_PROXY_HPP

class object;
class object_store;

struct object_proxy;

struct object_proxy {
  object_proxy(object *o = 0, object_store *os = 0);
  ~object_proxy();
  
  void remove();
  void insert(object_proxy* oproxy);

  object_proxy *prev;
  object_proxy *next;
  object *obj;
  unsigned long id;
  
  object_store *ostore;
};

#endif /* OBJECT_PROXY_HPP */
