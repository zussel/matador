#ifndef MATADOR_DI_HPP
#define MATADOR_DI_HPP

#include "matador/utils/singleton.hpp"

#include <memory>
#include <utility>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <typeindex>
#include <thread>
#include <functional>
#include <stdexcept>

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
   * @brief Acquires an instance
   *
   * Acquires an instance based on the internal strategy
   * and returns an anonymous pointer to the object
   *
   * @return The current injected object depending of the strategy
   */
  virtual void *acquire() = 0;
};

/**
 * Implements the transient dependency injection
 * strategy. When ever injected a new instance
 * is created.
 *
 * @tparam T Type of the object to be injected
 */
template<class T>
class transient_strategy : public strategy
{
public:
  /**
   * @brief Construct a new transient strategy object
   *
   * @tparam Args Type of arguments for constructor
   * @param args Arguments for constructor
   */
  template<typename ...Args>
  explicit transient_strategy(Args &&...args) {
    creator_ = [args..., this]() {
      instances_.push_back(std::make_unique<T>(args...));
      return instances_.back().get();
    };
  }

  void *acquire() override {
    return creator_();
  }

private:
  std::function<T *()> creator_{};
  std::vector<std::unique_ptr<T>> instances_;
};

/**
 * @brief Implements the singleton strategy
 *
 * The singleton strategy provides only one
 * instance of the type.
 *
 * @tparam T Type of the object to be injected
 */
template<class T>
class singleton_strategy : public strategy
{
public:
  template<typename ...Args>
  explicit singleton_strategy(Args &&...args) {
    creator_ = [args..., this]() {
      if (!instance_) {
        instance_ = std::make_unique<T>(args...);
      }
      return instance_.get();
    };
  }

  void *acquire() override {
    return creator_();
  }

private:
  std::function<T *()> creator_{};
  std::unique_ptr<T> instance_;
};

template<class T>
class singleton_per_thread_strategy : public strategy
{
public:
  template<typename ...Args>
  explicit singleton_per_thread_strategy(Args &&...args) {
    creator_ = [args..., this]() {
      thread_local auto instance = std::make_unique<T>(args...);
      std::lock_guard<std::mutex> lock(instance_mutex_);
      auto it = instance_map_.insert({std::this_thread::get_id(), std::move(instance)}).first;
      return it->second.get();
    };
  }

  void *acquire() override {
    return creator_();
  }

private:
  std::function<T *()> creator_{};
  std::mutex instance_mutex_;
  std::unordered_map<std::thread::id, std::unique_ptr<T>> instance_map_;
};
/**
 * @brief Provides a specific instance on injection
 *
 * @tparam T
 */
template<class T>
class instance_strategy : public strategy
{
public:
  explicit instance_strategy(T &&obj)
  : instance_(obj) {}

  void* acquire() override {
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
  T* get() const {
    return static_cast<T *>(strategy_->acquire());
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
  void to( Args &&...args) {
    initialize_strategy(std::make_unique<transient_strategy<T>>(std::forward<Args &&>(args)...));
  }

  template<typename T>
  void to_instance(T &&obj) {
    initialize_strategy(std::make_unique<instance_strategy<T>>(obj));
  }

  template<typename T, typename ...Args, typename std::enable_if<std::is_base_of<I, T>::value>::type * = nullptr>
  void to_singleton(Args &&...args) {
    initialize_strategy(std::make_unique<singleton_strategy<T>>(std::forward<Args &&>(args)...));
  }

  template<typename T, typename ...Args, typename std::enable_if<std::is_base_of<I, T>::value>::type * = nullptr>
  void to_singleton_per_thread(Args &&...args) {
    initialize_strategy(std::make_unique<singleton_per_thread_strategy<T>>(std::forward<Args &&>(args)...));
  }
};

class module
{
private:
  using t_type_proxy_map = std::unordered_map<std::type_index, std::shared_ptr<proxy_base>>;

public:
  void clear()
  {
    default_map_.clear();
    module_map_.clear();
  }

  template<typename I>
  std::shared_ptr<proxy<I>> bind() {
    return bind<I>(default_map_);
  }

  template<typename I>
  std::shared_ptr<proxy<I>> bind(const std::string &name) {
    auto i = module_map_.find(name);
    if (i == module_map_.end()) {
      i = module_map_.insert(std::make_pair(name, t_type_proxy_map{})).first;
    }
    return bind<I>(i->second);
  }

  template<typename I>
  I* resolve() {
    return resolve<I>(default_map_);
  }

  template<typename I>
  I* resolve(const std::string &name) {
    auto i = module_map_.find(name);
    if (i == module_map_.end()) {
      throw std::logic_error("unknown name " + name);
    }
    return resolve<I>(i->second);
  }

private:
  template<typename I>
  std::shared_ptr<proxy<I>> bind(t_type_proxy_map &type_proxy_map) {
    auto di_proxy_ptr = std::make_shared<proxy<I>>();
    auto i = type_proxy_map.insert(std::make_pair(std::type_index(typeid(I)), di_proxy_ptr));
    return std::static_pointer_cast<proxy<I>>(i.first->second);
  }

  template<typename I>
  I* resolve(t_type_proxy_map &type_proxy_map) {
    auto i = type_proxy_map.find(std::type_index(typeid(I)));
    if (i == type_proxy_map.end()) {
      throw std::logic_error(std::string("couldn't find type ") + typeid(I).name());
    }
    return i->second->get<I>();
  }

private:
  t_type_proxy_map default_map_;

  std::unordered_map<std::string, t_type_proxy_map> module_map_ {};
};

class repository : public singleton<repository>
{
public:
  void clear()
  {
    module_.clear();
  }

  void install(const std::function<void(module&)>& builder)
  {
    module_.clear();
    builder(module_);
  }

  void append(const std::function<void(module&)>& builder)
  {
    builder(module_);
  }

  template<typename I>
  I* resolve() {
    return module_.resolve<I>();
  }

  template<typename I>
  I* resolve(const std::string &name) {
    return module_.resolve<I>(name);
  }

private:
  module module_;
};

/**
 * @brief Resolves and provides the requested service
 *
 * The class inject acts a holder for the
 * requested service. On construction it
 * tries to resolve the given template type
 * within the global service repository.
 *
 * @tparam T Type of the interface
 */
template<class T>
class inject
{
public:
  inject()
  : obj(repository::instance().resolve<T>())
  {}

  explicit inject(const std::string &name)
  : obj(repository::instance().resolve<T>(name))
  {}

  explicit inject(module &m)
  : obj(m.resolve<T>())
  {}

  inject(module &m, const std::string &name)
  : obj(m.resolve<T>(name))
  {}

  inject(const inject &x)
  : obj(x.obj)
  {}

  inject& operator=(const inject &x)
  {
    if (this != *x) {
      obj = x.obj;
    }
    return *this;
  }

  inject(inject &&x) noexcept
  : obj(x.obj)
  {
  x.obj = nullptr;
  }

  inject& operator=(inject &&x) noexcept
  {
    obj = x.obj;
    x.obj = nullptr;
    return *this;
  }

  T* operator->() const
  {
    return obj;
  }

  T* get() const
  {
    return obj;
  }
  T& operator*() const
  {
    return *obj;
  }
private:
  T* obj;
};

void clear_module()
{
  repository::instance().clear();
}

void install_module(const std::function<void(module&)>& builder)
{
  repository::instance().clear();
  repository::instance().install(builder);
}

void append_module(const std::function<void(module&)>& builder)
{
  repository::instance().append(builder);
}

}

}

#endif //MATADOR_DI_HPP
