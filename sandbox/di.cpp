#include <iostream>
#include <unordered_map>
#include <typeindex>
#include <utility>
#include <functional>

struct di_item
{
  ~di_item()
  {
    if (deleter_) {
      deleter_(object);
    }
  }

  template<typename T>
  void use(T &obj)
  {
    // Todo: type id checking
    object = &obj;
  }

  template<typename T, typename ...Args>
  void to_singleton_move(Args&& ...args)
  {
    creator_ = [&args...]() {
      std::cout << "creating " << typeid(T).name() << "\n";
      return (void*)(new T(std::forward<Args>(args)...));
    };
    deleter_ = &destroy<T>;
  }

  template<typename T, typename ...Args>
  void to_singleton_copy(Args ...args)
  {
    creator_ = [&args...]() {
      std::cout << "creating " << typeid(T).name() << "\n";
      return (void*)(new T(args...));
    };
    deleter_ = &destroy<T>;
  }

  template<typename T>
  T& get()
  {
    if (!object) {
      if (!creator_) {
        throw std::logic_error("no creator function set");
      }
      object = creator_();
    }
    return *static_cast<T*>(object);
  }

  template <typename T>
  static void destroy(void* p)
  {
    delete (T*)p;
  }

  typedef void (*deleter)(void*);

  std::function<void*()> creator_ {};
  deleter deleter_ = nullptr;

  void *object = nullptr;
};

struct di {

  template < typename T >
  static di_item& bind() {
    auto i = item_map.insert(std::make_pair(std::type_index(typeid(T)), di_item{}));
    return i.first->second;
  }

  template < typename T >
  static T& resolve()
  {
    auto i = item_map.find(std::type_index(typeid(T)));
    if (i == item_map.end()) {
      throw std::logic_error(std::string("couldn't find type ") + typeid(T).name());
    }
    return i->second.get<T>();

  }
  static std::unordered_map<std::type_index, di_item> item_map;
};

std::unordered_map<std::type_index, di_item> di::item_map = {};

template < class T >
struct inject
{
  inject()
    : obj(di::resolve<T>())
  {

  }

  T* operator->() {
    return &obj;
  }

  T &obj;
};

struct service
{
  service(std::string n, long i)
    : name(std::move(n)), id(i)
  {}

  void print() {
    std::cout << "I'm service " << name.c_str() << " (id: " << id << ", "<< this << ")\n";
  }

  std::string name;
  long id;
};

struct another_service
{

  void dump()
  {
    std::cout << "I'm another service (" << this << ")\n";
  }
};

struct super_service
{
  super_service(std::string n, long i)
    : name(std::move(n)), id(i)
  {}

  void print() {
    std::cout << "I'm super service " << name.c_str() << " (id: " << id << ", "<< this << ")\n";
  }

  std::string name;
  long id;

};

struct client
{
  inject<service> s;
};

int main()
{
  another_service s {};
  di::bind<another_service>().use(s);
  di::bind<service>().to_singleton_move<service>("test", 3);

  std::string name { "super "};
  long id { 2 };

  di::bind<super_service>().to_singleton_copy<super_service>(name, id);

  inject<service> si;
  inject<another_service> asi;
  inject<super_service> ssi;

  si->print();

  asi->dump();

  ssi->print();

  client c;

  c.s->print();
}
