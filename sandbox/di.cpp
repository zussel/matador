#include <iostream>
#include <unordered_map>
#include <typeindex>
#include <utility>
#include <functional>
#include <vector>
#include <memory>

class di_strategy
{
public:
  virtual ~di_strategy() = default;

  virtual void* create() = 0;
};

template < class T >
class di_default : public di_strategy
{
public:
  template < typename ...Args >
  explicit di_default(Args&& ...args)
  {
    creator_ = [&args..., this]() {
      std::cout << "creating " << typeid(T).name() << "\n";
      instances_.push_back(std::make_unique<T>(std::forward<Args&&>(args)...));
      return instances_.back().get();
    };
  }

  void* create() override
  {
    return creator_();
  }

private:
  std::function<T*()> creator_ {};
  std::vector<std::unique_ptr<T>> instances_;
};

template < class T >
class di_singleton : public di_strategy
{
public:
  template < typename ...Args >
  explicit di_singleton(Args&& ...args)
  {
    creator_ = [&args..., this]() {
      if (!instance_) {
        std::cout << "creating " << typeid(T).name() << "\n";
        instance_ = std::make_unique<T>(std::forward<Args&&>(args)...);
      }
      return instance_.get();
    };
  }

  void* create() override
  {
    return creator_();
  }

private:
  std::function<T*()> creator_ {};
  std::unique_ptr<T> instance_;
};

template < class T >
class di_instance : public di_strategy
{
public:
  explicit di_instance(T &&obj)
    : instance_(obj)
  {}

  void* create() override
  {
    return &instance_;
  }

private:
  T &instance_;
};

struct di_proxy
{
  template<typename T, typename ...Args>
  void to(Args&& ...args) {
    strategy_ = std::make_unique<di_default<T>>(std::forward<Args&&>(args)...);
  }

  template<typename T>
  void to_instance(T &&obj)
  {
    strategy_ = std::make_unique<di_instance<T>>(obj);
  }

  template<typename T, typename ...Args>
  void to_singleton(Args&& ...args)
  {
    strategy_ = std::make_unique<di_singleton<T>>(std::forward<Args&&>(args)...);
  }

  template<typename T>
  T& get()
  {
    return *static_cast<T*>(strategy_->create());
  }

  std::unique_ptr<di_strategy> strategy_;
};

struct di {

  template < typename T >
  di_proxy& bind() {
    auto i = item_map.insert(std::make_pair(std::type_index(typeid(T)), di_proxy{}));
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
  std::unordered_map<std::type_index, di_proxy> item_map {};
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
  explicit car(int num_tyres) : num_tyres_(num_tyres) {}

    void print() override {
      std::cout << "I'm car " << this << " with " << num_tyres_ << " tyres\n";

    }

    int num_tyres_ {};
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

  mydi.bind<icar>().to<car>(4);
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
