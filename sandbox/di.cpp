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

  template< typename T >
  void to() {
    creator_ = [](void *) {
      std::cout << "creating " << typeid(T).name() << "\n";
      return (void*)(new T);
    };
  }

  template<typename T>
  void to_instance(T &&obj)
  {
    creator_ = [&obj](void *) {
      return (void*)&std::forward<T&&>(obj);
    };
    // Todo: type id checking
//    object = &obj;
  }

  template<typename T, typename ...Args>
  void to_singleton(Args&& ...args)
  {
    creator_ = [&args...](void *obj) {
      if (obj) {
        return obj;
      }
      std::cout << "creating " << typeid(T).name() << "\n";
      return (void*)(new T(std::forward<Args&&>(args)...));
    };
    deleter_ = &destroy<T>;
  }

  template<typename T>
  T& get()
  {
    return *static_cast<T*>(creator_(object));
  }

  template <typename T>
  static void destroy(void* p)
  {
    delete (T*)p;
  }

  typedef void (*deleter)(void*);

  std::function<void*(void*)> creator_ {};
  deleter deleter_ = nullptr;

  void *object = nullptr;
};

struct di {

  struct di_bind_mediator
  {
    explicit di_bind_mediator(di &d) : di_(d) {}

    void to()
    {

    }

    void to_singleton() {}

    di &di_;
  };

  template < typename T >
  di_item& bind() {
    auto i = item_map.insert(std::make_pair(std::type_index(typeid(T)), di_item{}));
    return i.first->second;
  }

  template < typename T >
  T& resolve()
  {
    auto i = item_map.find(std::type_index(typeid(T)));
    if (i == item_map.end()) {
      throw std::logic_error(std::string("couldn't find type ") + typeid(T).name());
    }
    return i->second.get<T>();

  }
  std::unordered_map<std::type_index, di_item> item_map {};
};

static di mydi;

template < class T >
struct inject
{
  inject()
    : obj(mydi.resolve<T>())
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

struct icar
{
    virtual ~icar() = default;

    virtual void print() = 0;
};
struct car : public icar
{
    void print() override {
      std::cout << "I'm car " << this << "\n";

    }
};

int main()
{
  {
    another_service s{};
    mydi.bind<another_service>().to_instance(s);
  }
  mydi.bind<service>().to_singleton<service>("test", 3);

  {
    std::string name{"super "};
    long id{2};

    mydi.bind<super_service>().to_singleton<super_service>(name, id);
  }

  mydi.bind<icar>().to<car>();
  inject<service> si;
  inject<another_service> asi;
  inject<super_service> ssi;

  si->print();

  asi->dump();

  ssi->print();

  client c;

  c.s->print();

  inject<icar> car1;
  inject<icar> car2;
  car1->print();
  car2->print();
}
