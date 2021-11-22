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

struct di_proxy_base
{
public:
  virtual ~di_proxy_base() = default;

  template<typename T>
  T& get()
  {
    return *static_cast<T*>(strategy_->create());
  }

protected:
  void initialize_strategy(std::unique_ptr<di_strategy> &&strategy)
  {
    strategy_ = std::move(strategy);
  }

private:
  std::unique_ptr<di_strategy> strategy_;
};

template < typename I >
struct di_proxy :public di_proxy_base
{
  template<typename T, typename ...Args, typename std::enable_if<std::is_base_of<I, T>::value>::type* = nullptr >
  void to(Args&& ...args) {
    initialize_strategy(std::make_unique<di_default<T>>(std::forward<Args&&>(args)...));
  }

  template<typename T>
  void to_instance(T &&obj)
  {
    initialize_strategy(std::make_unique<di_instance<T>>(obj));
  }

  template<typename T, typename ...Args>
  void to_singleton(Args&& ...args)
  {
    initialize_strategy(std::make_unique<di_singleton<T>>(std::forward<Args&&>(args)...));
  }
};

struct di {

  template < typename I >
  std::shared_ptr<di_proxy<I>> bind() {
    auto di_proxy_ptr = std::make_shared<di_proxy<I>>();
    auto i = item_map.insert(std::make_pair(std::type_index(typeid(I)), di_proxy_ptr));
    return std::static_pointer_cast<di_proxy<I>>(i.first->second);
  }

  template < typename I >
  I& resolve()
  {
    auto i = item_map.find(std::type_index(typeid(I)));
    if (i == item_map.end()) {
      throw std::logic_error(std::string("couldn't find type ") + typeid(I).name());
    }
    return i->second->get<I>();

  }
  std::unordered_map<std::type_index, std::shared_ptr<di_proxy_base>> item_map {};
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
  car() : num_tyres_(4) {}
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
    mydi.bind<another_service>()->to_instance(s);
  }
  mydi.bind<service>()->to_singleton<service>("test", 3);

  {
    std::string name{"super "};
    long id{2};

    mydi.bind<super_service>()->to_singleton<super_service>(name, id);
  }

  mydi.bind<icar>()->to<car>();
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
