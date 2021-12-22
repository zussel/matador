#include <iostream>
#include <unordered_map>
#include <typeindex>
#include <utility>
#include <functional>
#include <vector>
#include <memory>

#include "matador/utils/singleton.hpp"

template <typename Arg, typename... Args>
void doPrint(std::ostream& out, Arg&& arg, Args&&... args)
{
  out << std::forward<Arg>(arg);
  using expander = int[];
  (void)expander{0, (void(out << ", " << std::forward<Args>(args) << " "), 0)...};
}

namespace matador {
namespace di {
/**
 * Interface for the dependency injection
 * creation strategy.
 *
 * The injected service
 */
class strategy
{
public:
  /**
   * Destructor
   */
  virtual ~strategy() = default;

  /**
   *
   * @return The current injected object depending of the strategy
   */
  virtual void *create() = 0;
};

/**
 * Implements the default dependency injection
 * strategy. When ever injected a new instance
 * is created.
 *
 * @tparam T Type of the object to be injected
 */
template<class T>
class default_strategy : public strategy
{
public:
  template<typename ...Args>
  explicit default_strategy(Args &&...args) {
    creator_ = [args..., this]() {
      std::cout << "creating " << typeid(T).name() << "\n";
      instances_.push_back(std::make_unique<T>(args...));
      return instances_.back().get();
    };
  }

  void *create() override {
    return creator_();
  }

private:
  std::function<T *()> creator_{};
  std::vector<std::unique_ptr<T>> instances_;
};

template<class T>
class singleton_strategy : public strategy
{
public:
  template<typename ...Args>
  explicit singleton_strategy(Args &&...args) {
    //doPrint(std::cout, args...);
    creator_ = [args..., this]() {
      if (!instance_) {
        std::cout << "creating " << typeid(T).name() << "\n";
        doPrint(std::cout, args...);
        std::cout << "\n";
        instance_ = std::make_unique<T>(args...);
      }
      return instance_.get();
    };
  }

  void *create() override {
    return creator_();
  }

private:
  std::function<T *()> creator_{};
  std::unique_ptr<T> instance_;
};

template<class T>
class instance_strategy : public strategy
{
public:
  explicit instance_strategy(T &&obj)
    : instance_(obj) {}

  void *create() override {
    return &instance_;
  }

private:
  T &instance_;
};

class proxy_base
{
public:
  virtual ~proxy_base() = default;

  template<typename T>
  T *get() {
    return static_cast<T *>(strategy_->create());
  }

protected:
  void initialize_strategy(std::unique_ptr<strategy> &&strategy) {
    strategy_ = std::move(strategy);
  }

private:
  std::unique_ptr<strategy> strategy_;
};

template<typename I>
class proxy : public proxy_base
{
public:
  template<typename T, typename ...Args, typename std::enable_if<std::is_base_of<I, T>::value>::type * = nullptr>
  void to(Args &&...args) {
    initialize_strategy(std::make_unique<default_strategy<T>>(std::forward<Args &&>(args)...));
  }

  template<typename T>
  void to_instance(T &&obj) {
    initialize_strategy(std::make_unique<instance_strategy<T>>(obj));
  }

  template<typename T, typename ...Args, typename std::enable_if<std::is_base_of<I, T>::value>::type * = nullptr>
  void to_singleton(Args &&...args) {
    doPrint(std::cout, args...);
    initialize_strategy(std::make_unique<singleton_strategy<T>>(std::forward<Args &&>(args)...));
  }
};

class module
{
public:
  template<typename I>
  std::shared_ptr<proxy<I>> bind() {
    auto di_proxy_ptr = std::make_shared<proxy<I>>();
    auto i = item_map.insert(std::make_pair(std::type_index(typeid(I)), di_proxy_ptr));
    return std::static_pointer_cast<proxy<I>>(i.first->second);
  }

  template<typename I>
  I* resolve() {
    auto i = item_map.find(std::type_index(typeid(I)));
    if (i == item_map.end()) {
      throw std::logic_error(std::string("couldn't find type ") + typeid(I).name());
    }
    return i->second->get<I>();

  }

private:
  std::unordered_map<std::type_index, std::shared_ptr<proxy_base>> item_map{};
};

class module_builder
{
public:
  virtual ~module_builder() = default;

  virtual void build(module &module) = 0;
};

class repository : public matador::singleton<repository>
{
public:
  void install_module(std::unique_ptr<module_builder> &&builder) {
    builder->build(module_);
  }

  template<typename I>
  I* resolve() {
    return module_.resolve<I>();
  }

private:
  module module_;
};


template<class T>
class inject
{
public:
  inject()
    : obj(repository::instance().resolve<T>()) {}

  T *operator->() {
    return obj;
  }

  const T *operator->() const {
    return &obj;
  }

private:
  T *obj;
};

void install_module(std::unique_ptr<module_builder> &&builder)
{
  repository::instance().install_module(std::move(builder));
}

}
}

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
  matador::di::inject<service> s;
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

class test_module_builder : public matador::di::module_builder
{
public:
  void build(matador::di::module &module) override {
    {
      another_service s{};
      module.bind<another_service>()->to_instance(s);
    }
    module.bind<service>()->to_singleton<service>("test", 3);
    {
      std::string name{"super "};
      long id{2};

      module.bind<super_service>()->to_singleton<super_service>(name, id);
    }

    module.bind<icar>()->to<car>(4);
  }
};

using namespace matador::di;

int main()
{
  matador::di::install_module(std::make_unique<test_module_builder>());

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
